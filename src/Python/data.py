import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.special import erfinv

# ----- CONFIGURATION ESTHÉTIQUE -----
sns.set(style="whitegrid", palette="muted", font_scale=1.1)
plt.rcParams["figure.figsize"] = (8, 5)

# ----- LECTURE DU FICHIER CSV -----
# Remplacez le chemin par le vôtre
# On précise decimal=',' pour gérer les virgules décimales dans un CSV à virgules
# Le moteur 'python' gère mieux ce cas

df = pd.read_csv(
    "../../SY15_data.csv",
    sep=",",  # séparateur de colonnes
    header=0,
    decimal=",",  # virgule décimale
    engine='python'
)

# ----- LISTE DES COLONNES À TRAITER -----
# (nom_colonne, titre_pour_le_graph)
colonnes = [
    # ("AGV1 attente à prod1",      "AGV1 : attente -> prod1"),
    # ("AGV1 prod1 à warehouse",    "AGV1 : prod1 -> warehouse"),
    # ("AGV1 warehouse à attente",  "AGV1 : warehouse -> attente"),
    # ("AGV2 attente à warehouse",  "AGV2 : attente -> warehouse"),
    # ("AGV2 warehouse à client 2", "AGV2 : warehouse -> client 2"),
    # ("AGV2 client 2 à attente",   "AGV2 : client 2 -> attente"),
    # ("temps déposer",             "Temps déposer"),
    ("temps récupérer",           "Temps récupérer"),
]

# ----- PARAMÈTRES DE L'INTERVALLE DE CONFIANCE -----
alpha = 0.01   # niveau de confiance 99%
crit = np.sqrt(2) * erfinv(1 - alpha)

# ----- TRACÉ POUR CHAQUE COLONNE -----
for col, titre in colonnes:
    # extraction des données (suppression des NaN)
    data = df[col].dropna().values.astype(float)
    n = len(data)
    if n == 0:
        continue

    # moyenne et écart-type (population)
    mu = np.mean(data)
    sigma = np.std(data, ddof=0)

    # bornes pour la densité normale
    xmin, xmax = data.min(), data.max()
    x_vals = np.linspace(xmin, xmax, 200)
    pdf_vals = (1.0 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-0.5 * ((x_vals - mu) / sigma) ** 2)

    # calcul de l'intervalle de confiance
    marge = sigma / np.sqrt(n) * crit
    borne_inf = mu - marge
    borne_sup = mu + marge

    # création de la figure
    plt.figure()

    # histogramme normalisé en densité
    sns.histplot(
        data,
        bins=20,
        kde=False,
        stat="density",
        color="steelblue",
        edgecolor="white",
        alpha=0.7,
    )

    # courbe normale
    plt.plot(
        x_vals,
        pdf_vals,
        color="red",
        linestyle="-",
        linewidth=2,
        label=f"Loi Normale\nμ={mu:.2f}, σ={sigma:.2f}",
    )

    # bande de l'intervalle de confiance
    plt.axvspan(
        borne_inf,
        borne_sup,
        color="orange",
        alpha=0.3,
        label=f"CI {100*(1-alpha):.0f}% : [{borne_inf:.2f}, {borne_sup:.2f}]"
    )

    # ligne de la moyenne
    plt.axvline(
        mu,
        color="black",
        linestyle="--",
        linewidth=1.5,
        label=f"Moyenne = {mu:.2f}"
    )

    # titres et légendes
    plt.title(f"Histogramme & Loi Normale : {titre}")
    plt.xlabel(col)
    plt.ylabel("Densité")
    plt.legend(loc="upper right")

    plt.tight_layout()
    plt.show()

    # affichage console des stats
    print(f"Colonne «{col}» — n = {n}")
    print(f" Moyenne = {mu:.3f}, σ = {sigma:.3f}")
    print(f" Intervalle de confiance {100*(1-alpha):.0f}% : [{borne_inf:.3f} , {borne_sup:.3f}]\n")
