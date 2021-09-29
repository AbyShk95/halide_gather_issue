#include "Halide.h"

using namespace Halide;
class ResizeHalide : public Generator<ResizeHalide> {
public:
    Input<Buffer<uint8_t>> in{"in", 2};
    Input<int32_t> sX{"sX"};
    Input<int32_t> sY{"sY"};
    Output<Buffer<uint8_t>> out{"out", 2};

    void generate() {
        in_vtcm(x, y) = in(x, y);

        Expr x_coord = (x * sX) >> 10;
        Expr y_coord = (y * sY) >> 10;

        /**
         * Gather generation possible in following scenario:
         * A fixed sX and sY value; 2048 in this case.
         * 
         * Expr x_coord = (x * 2048) >> 10;
         * Expr y_coord = (y * 2048) >> 10;
         */ 

        out_vtcm(x,y) = in_vtcm(x_coord, y_coord);        
        
        out(x,y) = out_vtcm(x,y);

        out.dim(0).set_stride(1);
        in.dim(0).set_stride(1);

        in.dim(0).set_min(0);
        in.dim(1).set_min(0);
        out.dim(0).set_min(0);
        out.dim(1).set_min(0);

        in.set_host_alignment(128);
        out.set_host_alignment(128);
    }

    void schedule() {
            Var xi, yi;

            in_vtcm
                .compute_at(out, y)
                .vectorize(x, 128, TailStrategy::RoundUp)
                ;
            out_vtcm
                .compute_at(out, y)
                .vectorize(x, 128, TailStrategy::RoundUp)
                ;
            out
                .split(y, y, yi, 32)
                .parallel(y)
                .vectorize(x, 128)
                ;
            
            in_vtcm.store_in(MemoryType::VTCM);
            out_vtcm.store_in(MemoryType::VTCM);
        
    }

private:
    Func in_vtcm, out_vtcm;
    Var x{"x"}, y{"y"};

};

HALIDE_REGISTER_GENERATOR(ResizeHalide, resizeHalide);
