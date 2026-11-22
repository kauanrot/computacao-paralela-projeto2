#!/usr/bin/env python3
"""Gera os graficos do relatorio a partir dos CSVs do medir.sh.

Para cada problema com resultados.csv:
  graficos/<problema>_speedup.png     S x P, com a reta y = P (speedup ideal)
  graficos/<problema>_eficiencia.png  E x P, com a reta y = 1 (eficiencia ideal)
  graficos/<problema>_tabela.txt      tabela P | T | S | E pronta para copiar
Se o P8 tiver schedules.csv (comparar_schedule.sh):
  graficos/P8_mandelbrot_schedules.png

Uso (na pasta entrega/relatorio):
  pip install matplotlib        # uma vez
  python3 graficos.py
"""
import csv
import os
import sys

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ImportError:
    sys.exit("Falta o matplotlib: rode  pip install matplotlib")

AQUI = os.path.dirname(os.path.abspath(__file__))
ENTREGA = os.path.dirname(AQUI)
SAIDA = os.path.join(AQUI, "graficos")

PROBLEMAS = [
    ("P1_pi", "P1 - Cálculo de π", "Iniciante/P1_pi"),
    ("P5_life", "P5 - Jogo da Vida", "Intermediario/P5_life"),
    ("P8_mandelbrot", "P8 - Mandelbrot", "Avancado/P8_mandelbrot"),
]

AZUL = "#2a78d6"        # serie medida
CINZA = "#52514e"       # reta de referencia
TEXTO = "#0b0b0b"
GRADE = "#e4e3df"

plt.rcParams.update({
    "font.family": "DejaVu Sans",
    "font.size": 10,
    "axes.edgecolor": CINZA,
    "axes.labelcolor": TEXTO,
    "xtick.color": CINZA,
    "ytick.color": CINZA,
    "axes.spines.top": False,
    "axes.spines.right": False,
    "figure.dpi": 150,
})


def ler_resultados(caminho):
    with open(caminho, newline="") as f:
        linhas = list(csv.DictReader(f))
    return ([int(l["P"]) for l in linhas], [float(l["T"]) for l in linhas],
            [float(l["S"]) for l in linhas], [float(l["E"]) for l in linhas])


def eixo_base(ax, ps):
    ax.set_xticks(ps)
    ax.set_xlabel("Número de threads (P)")
    ax.grid(axis="y", color=GRADE, linewidth=0.8)
    ax.set_axisbelow(True)


def grafico(ps, valores, ideal, titulo, rotulo_y, nome_ideal, arquivo, fmt, legenda="upper left"):
    fig, ax = plt.subplots(figsize=(6, 3.6))
    ax.plot(ps, ideal, linestyle="--", color=CINZA, linewidth=1.5, label=nome_ideal)
    ax.plot(ps, valores, color=AZUL, linewidth=2, marker="o", markersize=7, label="Medido")
    for p, v in zip(ps, valores):
        ax.annotate(fmt.format(v), (p, v), textcoords="offset points", xytext=(0, 8),
                    ha="center", fontsize=9, color=TEXTO)
    eixo_base(ax, ps)
    ax.set_ylabel(rotulo_y)
    ax.set_title(titulo, loc="left", fontsize=11, color=TEXTO)
    topo = max(max(valores), max(ideal))
    ax.set_ylim(0, topo * 1.18)
    ax.legend(frameon=False, loc=legenda)
    fig.tight_layout()
    fig.savefig(arquivo)
    plt.close(fig)


def grafico_schedules(caminho, arquivo):
    with open(caminho, newline="") as f:
        linhas = list(csv.DictReader(f))
    nomes = [l["schedule"] for l in linhas]
    tempos = [float(l["T"]) for l in linhas]
    fig, ax = plt.subplots(figsize=(6, 3.4))
    barras = ax.bar(nomes, tempos, color=AZUL, width=0.55)
    for b, t in zip(barras, tempos):
        ax.annotate(f"{t:.2f} s", (b.get_x() + b.get_width() / 2, t), textcoords="offset points",
                    xytext=(0, 4), ha="center", fontsize=9, color=TEXTO)
    ax.set_ylabel("Tempo (s)")
    ax.set_xlabel("Política de escalonamento (OMP_SCHEDULE)")
    ax.set_title("P8 - Tempo por schedule (mesmo P)", loc="left", fontsize=11, color=TEXTO)
    ax.grid(axis="y", color=GRADE, linewidth=0.8)
    ax.set_axisbelow(True)
    ax.set_ylim(0, max(tempos) * 1.15)
    fig.tight_layout()
    fig.savefig(arquivo)
    plt.close(fig)


def main():
    os.makedirs(SAIDA, exist_ok=True)
    feito = 0
    for chave, titulo, pasta in PROBLEMAS:
        csv_res = os.path.join(ENTREGA, pasta, "resultados.csv")
        if not os.path.exists(csv_res):
            print(f"  {chave}: sem resultados.csv (rode ./medir.sh na pasta) - pulando")
            continue
        ps, ts, ss, es = ler_resultados(csv_res)
        grafico(ps, ss, ps, f"{titulo} - Speedup", "S(P) = T(1) / T(P)", "Ideal (y = P)",
                os.path.join(SAIDA, f"{chave}_speedup.png"), "{:.2f}")
        grafico(ps, es, [1.0] * len(ps), f"{titulo} - Eficiência", "E(P) = S(P) / P",
                "Ideal (y = 1)", os.path.join(SAIDA, f"{chave}_eficiencia.png"), "{:.2f}",
                legenda="lower left")
        with open(os.path.join(SAIDA, f"{chave}_tabela.txt"), "w", encoding="utf-8") as f:
            f.write("P\tT(P) (s)\tS(P)\tE(P)\n")
            for p, t, s, e in zip(ps, ts, ss, es):
                f.write(f"{p}\t{t:.4f}\t{s:.2f}\t{e:.2f}\n")
        print(f"  {chave}: speedup, eficiência e tabela gerados")
        feito += 1
    csv_sch = os.path.join(ENTREGA, "Avancado/P8_mandelbrot/schedules.csv")
    if os.path.exists(csv_sch):
        grafico_schedules(csv_sch, os.path.join(SAIDA, "P8_mandelbrot_schedules.png"))
        print("  P8_mandelbrot: gráfico de schedules gerado")
    print(f"== {feito} problema(s); arquivos em {SAIDA}")


if __name__ == "__main__":
    main()
