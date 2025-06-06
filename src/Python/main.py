import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

# ----- CONFIGURATION ESTHÉTIQUE -----
sns.set(style="whitegrid", palette="muted", font_scale=1.1)
plt.rcParams["figure.figsize"] = (8, 5)

# ----- LECTURE DU FICHIER CSV -----
df = pd.read_csv("../../results.csv", sep=",", header=0)

# ----- LISTE DES COLONNES À TRAITER -----
colonnes = [
    ("temps total", "Temps total"),
    ("temps attente agv2", "Temps d'attente AGV2"),
    ("stock max warehouse", "Stock max dans le warehouse")
]

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
        bins=20,
        kde=False,
        stat="density",  # Normalise pour obtenir une densité
        color="steelblue",
        edgecolor="white",
        alpha=0.7
    )

    # 2) Courbe normale en rouge
    if col_name != "stock max warehouse":
        plt.plot(
            x_vals,
            pdf_vals,
            color="red",
            linestyle="-",
            linewidth=2,
            label="Loi normale\n$\mu={:.2f}$\n$\sigma={:.2f}$".format(mu, sigma)
        )

    # 3) Ligne verticale à la moyenne (optionnel si vous voulez la repère mu)
    plt.axvline(mu, color="orange", linestyle="--", linewidth=1.5, label=f"Moyenne = {mu:.2f}")

    # Titres et légendes
    plt.title(f"Histogramme & courbe normale : {titre_fr} FIFO FIFO x4")
    plt.xlabel(col_name)
    plt.ylabel("Densité")
    plt.legend(loc="upper right")

    # Mise en page et affichage
    plt.tight_layout()
    plt.show()
