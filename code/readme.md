# Code — Base roulante

Ce dossier contient l'ensemble du code source développé pour le contrôle de la base roulante.

## Structure des dossiers

| Dossier | Description |
|---|---|
| `Ligne_droite/` | Déplacement en ligne droite sur une distance prédéterminée, avec correction par boucle de rétroaction, phases d'accélération, maintien de vitesse et décélération |
| `Tournant/` | Codes de rotation de la voiture |
| `Sauvagarde_m/` | Archives des versions non abouties ; `commande_de_la_voiture` contient le code complet et fonctionnel de la base roulante |
| `ADC/` | Échantillonnage, corrélation et décodage des instructions sonores |
| `Code_complet/` | Intégration de l'ADC et du contrôle de la base roulante (avec déviation résiduelle sur trajectoire rectiligne) |
| `Code_final/` | Code présenté à l'évaluation finale — basé sur `Code_complet` V5, avec correction de la différence de taille des deux roues |

## Fichiers racine

| Fichier | Description |
|---|---|
| `configuration.c` / `configuration.h` | Configuration matérielle partagée entre les modules |
| `A implementer.md` | Liste de fonctionnalités envisagées mais non implémentées |
