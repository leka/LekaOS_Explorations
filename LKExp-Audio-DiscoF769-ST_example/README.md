#LKExp-Audio-DiscoF769-ST_example

## Introduction

Pour jouer un fichier WAV sur le vibreur haptique, il faut prendre en main un exemple fonctionnel.

ST propose deux exemples audio avec STM32CubeF7: 

* [[SAI_AudioPlay](https://github.com/STMicroelectronics/STM32CubeF7/tree/master/Projects/STM32F769I-Discovery/Examples/SAI/SAI_AudioPlay)]
* [[Audio_playback_and_record](https://github.com/STMicroelectronics/STM32CubeF7/tree/master/Projects/STM32F769I-Discovery/Applications/Audio/Audio_playback_and_record)]

L'objectif ici est de faire fonctionner l'un des deux exemples

## Exemples

SAI_AudioPlay permet de jouer un fichier binaire en interface SAI (vers un Codec puis DAC).

Audio_playback_and_record permet de jouer un fichier WAV en interface SAI (vers un Codec puis DAC), et ce depuis une clé USB.

Dans le cadre de Leka, Audio_playback_and_record s'en rapproche le plus avec l'utilisation de fichier WAV. Il faut toutefois adapter l'utilisation d'une clé USB avec une carte SD et modifier l'interface SAI pour utiliser le DAC directement.

## Audio_playback_and_record

Le contenu du dossier s'utilise sous STM32CubeIDE. Son simple copier-coller ne suffira pas.  
Le plus simple est de reprendre le repo STM32CubeF7, de retrouver l'exemple de base (...\STM32CubeF7\Projects\STM32F769I-Discovery\Applications\Audio\Audio_playback_and_record) et de remplacer les fichiers sources et includes. Il faudra aussi ajouter les libs externe pour la carte SD dans les propriétés du projet.

Pour la carte SD, il faut mettre le(s) fichier(s) WAV à la racine. Mettre dans un dossier ne fonctionnera pas.

Après avoir compilé et flashé, il ne reste qu'à appuyer sur `Play` et sélectionner le fichier WAV à jouer.
