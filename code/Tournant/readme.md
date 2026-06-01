## V1 
Reprise du code ligne droite et ajout des fonctions v_avant() et v_arriere qui assurent la roation de la voiture.
### Etat :
Fonctionnel mais aucune régulation.
## V2
Ajout de régulation avec fonction "correction()" pour la rotation v_avant()\\
Coefficients de régulation sont à améliorer.
# V3
Création d'une fonction de régulation unique pour la rotation (correction_rot()). \\
Régulation basée sur l'erreur multiplié par un coefficient empirique.
## Concept 
1) 
Rotation de la voiture par la fonction ""v_avant()". Cette fonction appelle la fonction de "correction_rot()" dont le coefficient est spécialement adapté à la rotation mais dont la structure est (pour l'instant) identique à la fonction de régulation de marche en ligne droite. 
2) Le facteur "seuil = 9.95 *180*3" a été trouvé à partir de la valeur calculée théoriquement(11,677) qu'on avait obtenue à partir du diamètre de la roue et du nombre de tour moteur nécessaires à un tour de roue. 
Nous avons trouvé cette valeur(9.95) en effectuant des tours de (3x180°) et rectifié par essais-erreurs jusqu'à avoir une rotation de la voiture qui corresponde au nombre de degrés désirés(3x180°).

# Amélioration à prévoir
- Faire de nombreux test sur un sol gradué, et éventuellement sur un parcours mêlant ligne droite et rotation.

- Il faut également vérifier si le centre de la voiture est conservé durant la rotation.

- Vérifier que c'est fonctionnel sur la rotation dans l'autre sens(v_arrier()).