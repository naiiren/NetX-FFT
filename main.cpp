#include <print>
#include <format>

#define SIZED_MPINT_BACKEND boost::multiprecision::uint128_t
#include <nxsim/circuit.h>
#include <nxsim/circuit_parser.h>

using namespace nxon;

struct reg_rule final : impl::rule_impl {
    nxon::clock_t clk;
    source_t rst, in;
    sink_t out;

    reg_rule(nxon::clock_t clk, source_t rst, source_t in, sink_t out)
        : rule_impl(clk.dependencies(), clk.dependencies() + rst.dependencies() + in.dependencies(), out.outcomes()),
          clk(std::move(clk)), rst(std::move(rst)), in(std::move(in)), out(std::move(out)) {}

    impl::indirect_id_set perform(value_storage &values) const override {
        static id_set EMPTY_ID_SET;
        if (const auto rst_v = rst.get(values);
            static_cast<bool>(*rst_v)) [[likely]] {
            if (auto in_v = in.get(values); clk.triggered(values) && out.check(values, *in_v)) {
                values.delay([value = in_v.release(), out = out](auto &cur_values) {
                    out.put(cur_values, std::move(value));
                });
                return impl::indirect_id_set(out.outcomes());
            }
        }
        return impl::indirect_id_set(EMPTY_ID_SET);
    }

    static rule_t parse(const parse_context &ctx, const nlohmann::json &json) {
        const auto &input = json["input"];
        const auto &output = json["output"];

        return rule_t{new reg_rule(
            parse_clock(input.at(0), true),
            parse_source(input.at(1), ctx),
            parse_source(input.at(2), ctx),
            parse_sink(output.at(0), ctx)
        )};
    }
};

int main() {
    std::string json;
    std::getline(std::cin, json);
    auto ctx = parse_circuit(nlohmann::json::parse(json), {
        {"reg", std::function(reg_rule::parse)}
    });

    const auto current_time = std::chrono::system_clock::now();

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

    for (int i = 0 ; i != 65536; ++i) {
        ctx.flip_input_clocks();
        ctx.flip_input_clocks();

        for (int j = 0; j != 8; ++j) {
            char real_name[16], imag_name[16];
            std::snprintf(real_name, sizeof(real_name), "x[%d].real", j);
            std::snprintf(imag_name, sizeof(imag_name), "x[%d].imag", j);

            auto real = ctx.get_by_name(real_name);
            auto imag = ctx.get_by_name(imag_name);

            if (real > value_t{24, 0x3FFFFF}) {
                ctx.update_by_name(real_name, {24, 0});
            } else {
                const unsigned step_real[8] = {1, 1, 31, 1, 23,1, 6, 1};
                ctx.update_by_name(real_name, real + value_t{24, step_real[j]});
            }

            if (imag > value_t{24, 0x3FFFFF}) {
                ctx.update_by_name(imag_name, {24, 0});
            } else {
                const unsigned step_imag[8] = {2, 5, 3, 6, 4, 8, 11, 7};
                ctx.update_by_name(imag_name, imag + value_t{24, step_imag[j]});
            }
        }
        for (int j = 0; j != 8; ++j) {
            char real_name[16], imag_name[16];
            std::snprintf(real_name, sizeof(real_name), "y[%d].real", j);
            std::snprintf(imag_name, sizeof(imag_name), "y[%d].imag", j);

            std::print("{}\t{}\n", ctx.get_by_name(real_name), ctx.get_by_name(imag_name));
        }
        std::print("\n");
    }

    const auto end_time = std::chrono::system_clock::now();
    const std::chrono::duration<double> elapsed_seconds = end_time - current_time;
    std::print("Elapsed time: {}s\n", elapsed_seconds.count());
    return 0;
}