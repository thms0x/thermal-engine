
import json
from pathlib import Path

import matplotlib.pyplot as plt

PROJECT_ROOT = Path(__file__).resolve().parent.parent
RESULTS = PROJECT_ROOT / "results"

FILES = {
    "Native C++": RESULTS / "native.json",
    "hostfxr": RESULTS / "hostfxr.json",
    "Native AOT": RESULTS / "aot.json",
}


def load_mean_cpu_time(path, benchmark_name):
    with path.open(encoding="utf-8") as file:
        data = json.load(file)

    for entry in data["benchmarks"]:
        name = entry["name"]

        if (
            entry.get("aggregate_name") == "mean"
            and name.removesuffix("_mean") == benchmark_name
        ):
            return entry["cpu_time"], entry["time_unit"]

    raise ValueError(
        f"Mean result for '{benchmark_name}' not found in {path}"
    )


def to_nanoseconds(value, unit):
    factors = {
        "ns": 1,
        "us": 1_000,
        "ms": 1_000_000,
        "s": 1_000_000_000,
    }

    if unit not in factors:
        raise ValueError(f"Unsupported time unit: {unit}")

    return value * factors[unit]


def plot_comparison(
    benchmark_name,
    title,
    unit,
    output,
    files=None,
):
    if files is None:
        files = FILES

    names = []
    values = []

    for approach, filename in files.items():
        value, original_unit = load_mean_cpu_time(
            filename,
            benchmark_name,
        )

        nanoseconds = to_nanoseconds(value, original_unit)

        if unit == "ns":
            converted = nanoseconds
        elif unit == "us":
            converted = nanoseconds / 1_000
        else:
            raise ValueError(
                f"Unsupported display unit: {unit}"
            )

        names.append(approach)
        values.append(converted)


    fig, ax = plt.subplots(figsize=(8, 4.5))

    bars = ax.bar(names, values, width=0.55)

    ax.set_title(title, fontsize=14, pad=15)
    ax.set_ylabel(f"Mean CPU time ({unit})")
    ax.set_ylim(0, max(values) * 1.25)

    ax.grid(axis="y", alpha=0.2)
    ax.set_axisbelow(True)

    for bar, value in zip(bars, values):
        ax.text(
            bar.get_x() + bar.get_width() / 2,
            bar.get_height() + max(values) * 0.02,
            f"{value:.3f}",
            ha="center",
            va="bottom",
        )

    fig.tight_layout()

    fig.savefig(output, dpi=200)
    fig.savefig(output.with_suffix(".svg"))

    plt.close(fig)

    print(f"Saved {output}")
    print(f"Saved {output.with_suffix('.svg')}")


def main():
    RESULTS.mkdir(parents=True, exist_ok=True)

    plot_comparison(
        benchmark_name="BM_CalculatePower",
        title="Controller Method Invocation",
        unit="ns",
        output=RESULTS / "method_calls.png",
    )


    plot_comparison(
        benchmark_name="BM_FullSimulation",
        title="Full Thermal Simulation (100 Steps)",
        unit="us",
        output=RESULTS / "full_simulation.png",
    )

    plot_comparison(
        benchmark_name="BM_CreateDestroy",
        title="Managed Controller Lifecycle (Create + Destroy)",
        unit="ns",
        output=RESULTS / "controller_lifecycle.png",
        files={
            "hostfxr": FILES["hostfxr"],
            "Native AOT": FILES["Native AOT"],
        },
    )



if __name__ == "__main__":
    main()
