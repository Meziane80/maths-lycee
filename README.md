# Maths Lycee - projet d'application NumWorks

Projet source d'une application externe native EADK pour NumWorks/Epsilon.

## Contenu
- Nuage de points : moyennes, variances, ecarts-types, covariance et droite d'ajustement affine.
- Tableau a double entree : totaux et frequences globales.
- Fractions : simplification, operations, comparaison et decimal.
- Pourcentages : pourcentage, taux d'evolution, hausse/baisse, coefficient multiplicateur.
- Calcul litteral : images, antecedents, equation ax+b=c et identites remarquables.

## Construction
Cette archive contient le code source, pas encore le fichier compile `.nwa`.
La compilation requiert `arm-none-eabi-gcc`, Node.js/npm et nwlink.

Dans un terminal a la racine du projet :
    make build

Le fichier compile sera genere dans `output/MathsLycee.nwa`.

La documentation EADK indique que les apps externes sont des fichiers NWA et que la construction passe par le compilateur ARM et nwlink.
Consulter :
https://github.com/numworks/epsilon/tree/master/epsilon/external_apps
https://github.com/numworks/epsilon/tree/master/epsilon/eadk

## Commandes
- Fleches haut/bas : navigation
- EXE/OK : valider
- BACK : retour
- Dans les champs numeriques : chiffres, point, moins, EXE pour valider, BACK pour effacer/annuler
- Dans les resultats : gauche/droite pour changer de page

Note : ce projet est une premiere base native. La fonction de calcul litteral couvre les formes et equations affichees dans ses menus, pas un systeme de calcul symbolique general.
