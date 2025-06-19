import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.special import erfinv
from scipy.stats import gamma


# ----- CONFIGURATION ESTHÉTIQUE -----
sns.set(style="whitegrid", palette="muted", font_scale=1.1)
plt.rcParams["figure.figsize"] = (8, 5)

# ----- LECTURE DU FICHIER CSV -----
df = pd.read_csv("../../results.csv", sep=",", header=0)

# ----- LISTE DES COLONNES À TRAITER -----
colonnes = [
    ("temps total", "Temps total"),
    ("temps attente agv2", "Temps d'attente AGV2"),
    # ("stock max warehouse", "Stock max dans le warehouse")
]

# ----- TRACÉ DES HISTOGRAMMES AVEC COURBE NORMALE ET BANDE CI à 98% -----
alpha = 0.01
crit = np.sqrt(2) * erfinv(1 - alpha)

# ----- TRACÉ DES HISTOGRAMMES AVEC COURBE NORMALE -----
for col_name, titre_fr in colonnes:
    # Extraction des données
    data = df[col_name].dropna().values

    # Calcul des statistiques
    mu = np.mean(data)
    sigma = np.std(data, ddof=0)  # écart-type population

    # Définition des bornes pour la courbe normale
    xmin, xmax = data.min(), data.max()
    x_vals = np.linspace(xmin, xmax, 200)  # 200 points pour lisser la courbe
    # Formule de la densité d'une N(mu, sigma^2)
    pdf_vals = (1.0 / (sigma * np.sqrt(2 * np.pi))) * np.exp(-0.5 * ((x_vals - mu) / sigma) ** 2)

    # Création de la figure
    plt.figure()

    # 1) Histogramme en densité
    sns.histplot(
        data,
        bins=30,
        kde=False,
        stat="density",  # Normalise pour obtenir une densité
        color="steelblue",
        edgecolor="white",
        alpha=0.7
    )


    # plt.plot(
    #     x_vals,
    #     pdf_vals,
    #     color="red",
    #     linestyle="-",
    #     linewidth=2,
    #     label="Loi normale\n$\mu={:.2f}$\n$\sigma={:.2f}$".format(mu, sigma)
    # )
    # intervalle de confiance
    # Calcul de l'intervalle de confiance 98%
    n = len(data)
    marge = sigma / np.sqrt(n) * crit
    borne_inf = mu - marge
    borne_sup = mu + marge
    # Affichage dans la console
    print(f"Colonne « {col_name} » — n = {n}")
    print(f" Intervalle de confiance 98% : [{borne_inf:.3f} , {borne_sup:.3f}]\n")
    # Tracé de la bande CI
    plt.axvspan(borne_inf, borne_sup, color="red", alpha=0.2,
                label=f"CI 99% : [{borne_inf:.1f}, {borne_sup:.1f}]")

    ### GAMMA FITTING ###
    # Estimation des paramètres de la loi gamma
    shape, loc, scale = gamma.fit(data)
    # Définition des bornes pour la courbe
    xmin, xmax = data.min(), data.max()
    x_vals = np.linspace(xmin, xmax, 200)
    # Calcul de la densité de la loi gamma
    gamma_pdf = gamma.pdf(x_vals, shape, loc=loc, scale=scale)
    # 2) Courbe de densité gamma
    plt.plot(
        x_vals,
        gamma_pdf,
        color="green",
        linestyle="-",
        linewidth=2,
        label="Loi gamma\nshape={:.2f}\nscale={:.2f}".format(shape, scale)
    )


    # 3) Ligne verticale à la moyenne (optionnel si vous voulez la repère mu)
    plt.axvline(mu, color="orange", linestyle="--", linewidth=1.5, label=f"Moyenne = {mu:.2f}")

    # Titres et légendes
    plt.title(f"Histogramme & courbe normale : {titre_fr} x2 SPT LPT")
    plt.xlabel(col_name)
    plt.ylabel("Densité")
    plt.legend(loc="upper right")

    # Mise en page et affichage
    plt.tight_layout()
    plt.show()
