# Circuit

## Vue d'ensemble

Ce dossier décrit la chaîne d'acquisition analogique utilisée pour traiter le signal d'un microphone. Le microphone produit un courant faible qui est traité par une chaîne de 3 étages avant d'être numérisé par l'ADC du microcontrôleur.

Les fréquences utiles sont :
- **800 Hz** : bit 0 / start bit
- **1200 Hz** : bit 1

## Étages

- **Étage 1 - Préamplificateur transimpédance** : convertit le courant du microphone en tension avec un grand gain pour maximiser le SNR dès le début de la chaîne.
- **Étage 2 - Filtre passe-bande** : sélectionne les fréquences utiles (800 Hz et 1200 Hz) et rejette les parasites.
- **Étage 3 - Amplificateur inverseur** : amplifie le signal filtré avant numérisation.

## Tension de référence (REF = 1.65 V)

Un diviseur résistif suivi d'un suiveur de tension fournit le point milieu (1.65 V). Le suiveur est nécessaire afin d'avoir une impédance d'entrée faible pour le reste du circuit, pour ne pas influencer les différents pôles du circuit


## Choix d'amplificateur
 Initialement, un MCP601 avait été choisi. Finalement, on a opté pour un MCP6021
le MCP6021 a été retenu pour deux raisons principales :
 
1. **Faible bruit** — 8.7 nV/√Hz contre 29 nV/√Hz pour le MCP601, soit 3× moins de bruit. À grand gain, ce critère est déterminant pour ne pas dégrader le SNR en fin de chaîne.
2. **Bande passante suffisante** — GBW = 10 MHz. Avec un gain de 10, la bande passante utile est de 1 MHz, bien au-delà des fréquences de travail (800 Hz et 1200 Hz).
**IN+** est connecté au REF (1.65 V) pour centrer le signal de sortie au milieu de la plage d'alimentation single-supply 3.3 V.
 