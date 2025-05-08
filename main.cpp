#include <boost/process.hpp>
#include <nxsim/circuit_parser.h>

using namespace nxon;

int main() {
    std::string json;
    boost::process::ipstream out;
    boost::process::child process("nx compile /home/nairen/workspace/evaluation/netx/fft/_netx.toml --minimal --top fft8",
                                  boost::process::std_out > out,
                                  boost::process::std_err > boost::process::null);
    std::getline(out, json);
    process.wait();
    auto ctx = parse_circuit(nlohmann::json::parse(json));

    ctx.circuit.update(1, {1, 0});
    ctx.flip_input_clocks();
    ctx.flip_input_clocks();
    ctx.circuit.update(1, {1, 1});
    ctx.update_by_name("x[0].real", {24, 10});
    ctx.update_by_name("x[1].real", {24, 20});
    ctx.update_by_name("x[2].real", {24, 30});
    ctx.update_by_name("x[3].real", {24, 40});
    ctx.update_by_name("x[4].real", {24, 10});
    ctx.update_by_name("x[5].real", {24, 20});
    ctx.update_by_name("x[6].real", {24, 30});
    ctx.update_by_name("x[7].real", {24, 40});

    ctx.flip_input_clocks();
    ctx.flip_input_clocks();
    ctx.circuit.update(2, {1, 1});

    unsigned step_real[8] = {1, 1, 31, 1, 23,1, 6, 1};
    unsigned step_imag[8] = {2, 5, 3, 6, 4, 8, 11, 7};
    for (int i = 0 ; i != 65535; ++i) {
        ctx.flip_input_clocks();
        ctx.flip_input_clocks();

        for (int j = 0; j != 8; ++j) {
            std::cout << ctx.get_by_name(std::format("x[{}].real", j)) << " "
                      << ctx.get_by_name(std::format("x[{}].imag", j))
                      << std::endl;
        }

        for (int j = 0; j != 8; ++j) {
            auto real_name = std::format("x[{}].real", j);
            auto imag_name = std::format("x[{}].imag", j);

            auto real = ctx.get_by_name(real_name);
            auto imag = ctx.get_by_name(imag_name);

            if (real > value_t{24, 0x3FFFFF}) {
                ctx.update_by_name(real_name, {24, 0});
            } else {
                ctx.update_by_name(real_name, real + value_t{24, step_real[j]});
            }

            if (imag > value_t{24, 0x3FFFFF}) {
                ctx.update_by_name(imag_name, {24, 0});
            } else {
                ctx.update_by_name(imag_name, imag + value_t{24, step_imag[j]});
            }
        }
        for (int j = 0; j != 8; ++j) {
            std::cout << ctx.get_by_name(std::format("y[{}].real", j)) << " "
                      << ctx.get_by_name(std::format("y[{}].imag", j))
                      << std::endl;
        }
        std::cout << std::endl;
    }
    return 0;
}