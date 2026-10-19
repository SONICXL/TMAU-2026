import csv
import sys
import shutil
import numpy as np
from pathlib import Path

from matplotlib.ticker import MaxNLocator
import matplotlib.pyplot as plt


def main():
    script_dir = Path(__file__).resolve().parent

    if len(sys.argv) > 1:
        csv_path = Path(sys.argv[1])
        if not csv_path.is_absolute():
            csv_path = script_dir / csv_path
    else:
        csv_path = script_dir / "result.csv"

    if not csv_path.exists():
        print(f"Ошибка: файл {csv_path} не найден.")
        return 1

    with csv_path.open("r", newline="", encoding="utf-8-sig") as file:
        rows = list(csv.DictReader(file))

    if not rows:
        print("Ошибка: CSV-файл не содержит результатов.")
        return 1

    steps = [int(row["step"]) for row in rows]
    inputs = [float(row["u_tau"]) for row in rows]
    outputs = [float(row["y_next"]) for row in rows]

    model = rows[0]["model"]
    signal = rows[0]["signal"]

    model_names = {
        "Model 1.9": "Модель 1.9",
        "Model 2.1": "Модель 2.1",
        "Model 3.3": "Модель 3.3",
    }

    signal_names = {
        "Step": "ступенчатое",
        "Impulse": "импульсное",
        "Harmonic": "гармоническое",
    }

    print(f"\nРезультаты: {model_names.get(model, model)}")
    print(f"Входное воздействие: {signal_names.get(signal, signal)}\n")

    print(f"{'Шаг':>8} {'u(tau)':>14} {'y(tau+1)':>14}")
    print("-" * 38)

    for step, u, y in zip(steps, inputs, outputs):
        print(f"{step:8d} {u:14.4f} {y:14.4f}")

    fig, axes = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

    if signal == "Harmonic":
        # гладкая синусоида sin(tau) + дискретные значения u(tau) точками
        # шаг k соответствует tau = k - 1
        t = np.linspace(0, steps[-1] - 1, 500)
        axes[0].plot(t + 1, np.sin(t), "--", linewidth=1, alpha=0.6, label="sin(τ)")
        axes[0].plot(steps, inputs, "o", label="u(τ) на шагах")
        axes[0].legend(loc="upper right")
    else:
        axes[0].plot(steps, inputs, marker="o")
    axes[0].set_title(
        f"Входное воздействие: {signal_names.get(signal, signal)}"
    )
    axes[0].set_ylabel("u(tau)")
    axes[0].grid(True)

    axes[1].plot(steps, outputs, marker="o")
    axes[1].set_title(
        f"Результат моделирования: {model_names.get(model, model)}"
    )
    axes[1].set_xlabel("Номер шага")
    axes[1].set_ylabel("y(tau+1)")
    axes[1].grid(True)
    axes[1].xaxis.set_major_locator(MaxNLocator(integer=True))

    fig.tight_layout()

    # PNG сразу в папку doc (на уровень выше src)
    doc_dir = script_dir.parent / "doc"
    doc_dir.mkdir(exist_ok=True)
    base_name = f"{model.replace(' ', '_')}_{signal}"
    image_path = doc_dir / f"{base_name}.png"
    csv_copy = doc_dir / f"{base_name}.csv"
    fig.savefig(image_path, dpi=150, bbox_inches="tight")
    if csv_path.resolve() != csv_copy.resolve():
        shutil.copyfile(csv_path, csv_copy)
 
    print(f"\nГрафик сохранён в файле: {image_path}")
    print(f"Таблица сохранена в файле: {csv_copy}")

    plt.show()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())