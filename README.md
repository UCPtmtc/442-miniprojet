<h1 style="text-align: center;">
  compte rendu mini-projet 442
</h1>

---------------------------

Mon projet était d'essayer de lire une suite d'image à une vitesse suffisante pour lire un dessin animé. Durant les TPs nous avions un code basé sur le formatage de carte SD `FATFS` et une lecture par
le `DMA` permettant de lire "relativement vite" les images sur une carte SD externe. \
Cependant même avec cette méthode très rapide on ne peut pas dépasser les 3 images/seconde même avec le format d'image le plus léger (`BMP RGB 565`). On a donc besoin de faire un passage de lecture 
sur chaque images pour les stocker dans la `RAM`  locale qui permet une lecture et impression sur l'écran beaucoup plus rapide (largement suffisant pour faire du 24 frames/seconde).

---------------------------

Le programme que je livre réussi à faire cela pour 20 images qui sont stocké sur une partie non exploité de la `SD RAM` (adresse 0X C026 0000). La mémoire libre à cet endroit correspond à 5,8 Mo.
La solution que je n'ai pas réussi à mettre en place est de réussir à faire fonctionner la `NOR RAM` (en 0X 6000 0000) qui permet d'après ce que j'ai vu dans l'onglet `memory` d'accéder à environ 68 Mo (soit presque 230 images). le document `.ioc`  ne me permet pas d'activer cette partie de la mémoire et il semblerait qu'il aurait fallu creuser du côté du périphérique `QUAD SPI` pour voir comment faire fonctionner cette mémoire ce que je n'ai pas pris le temps de faire.

---------------------------

Pour le support d'image j'ai utilisé un extrait de clip musical en dessin animé dont un internaute dévoué à éxtrait chaque frame et mis à disposition sur Github <https://github.com/bleov/bad-apple-frames>.\

J'ai pu alors diminuer et convertir les images `.jpg` de la source dans le format leger `BMP RGB 565` via les commandes suivantes dans le shell linux :

**conversion jpg -> bmp :**

```
for f in *.jpg ; 
do convert "$f" -format \
-define bmp:format='bmp4' \
-define bmp:subtype='RGB565' \
-resize '480x272!' "$f".bmp ; done
```

**réduction du nombre d'images :**

```
counter=0;
for f in *.bmp ; 
do counter=$((counter+1));
  if [[ $(( $counter % 5 ))  -ne  0 ]]; then
    rm "$f"
fi ; done
```
