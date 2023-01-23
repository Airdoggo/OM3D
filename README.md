# OM3D
## AUTHORS
Guillaume Poisson et Jordan Failloux

## Lien vers le projet

https://github.com/Airdoggo/OM3D

## Travail accompli

### TPs
Les deux TPs sont disponibles sur la branche `TPs`. Les TPs sont également présents sur les autres branches, mais la version sur cette branche ne contient qu'eux.

### Occlusion Culling
L'occlusion culling est disponible sur la branche `forward-oc`. Cette version fonctionne en forward, et une version incomplète en deferred est disponible sur la branche `occlusion-culling`, mais elle n'est pas totalement fonctionnelle.

### Frustum AS et Tile Deferred
La frustum AS et le Tile Deferred shading sont disponibles sur la branche `master`.

## Options pendant l'exécution

Pour l'occlusion culling, un slider permet de modifier la valeur qui détermine si un objet est trop petit pour cull est disponible.

Pour le reste :
- Un choix de différentes vues de debug : Albedo, Normales, Depth, Bounding Volume Hierarchy et Tiles.
- Un slider permettant de controler le nombre maximal d'enfants par arbre de la BVH.
- Un slider permettant de contrôler quel niveau de la hiérarchie afficher lors de la vue de debug correspondante.
- Des informations sur le nombre d'objets culled et le nombre de bounding boxes dont la présence dans le frustum a été calculée.
