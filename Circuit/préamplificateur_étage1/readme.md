## schéma
---
![Schéma du préamplificateur](schéma_préamplificateur.png)
---
## Rôle

Convertir le courant faible du micro électret en tension amplifiée.

## Principe

Un amplificateur transimpédance (TIA) convertit directement un courant d'entrée en tension de sortie :

```
Vout = −R2 · I
```

## Composants

| Composant | Valeur | Rôle |
|---|---|---|
| R2 (feedback) | 240 kΩ | Fixe le gain transimpédance |
| C_feedback | 0.5 nF | Stabilise la boucle (pôle HF) |
| REF | 1.65 V | Offset |

## Choix de conception

Initialement, la résistance de rétroaction était de 60 kΩ, mais il fallait alors amplifier davantage en fin de chaîne, ce qui avait pour conséquence d'amplifier également le bruit introduit par les composants suivants. Pour obtenir un meilleur SNR, on a donc augmenté le gain au niveau du préamplificateur afin de réduire le gain nécessaire en aval. De cette façon, le signal utile est largement dominant dès le premier étage, et les bruits additifs des étages suivants deviennent négligeables par rapport au signal, ce qui optimise le SNR

## Alimentation

- VDD : 3.3 V
- VSS : 0 V (GND)
- REF : 1.65 V (diviseur résistif + suiveur)

La capacité C3 et la résistance R1 servent à filtrer les basses fréquences (inférieures à 800 Hz).