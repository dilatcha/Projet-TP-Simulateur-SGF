#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>




//déclaration des constants nécessaire:

#define FB 6 //facteur de blocage

#define NbrBlocs 256 //nombre de blocs maximale pour la memoire secrondaire.

#define data_size 140 /*chaque enregistrement contient un tableau de charachtéres pour stocker des infromations multiples,
    le data_size représente la taille maximale de ce tableau*/

#define nbrMeta_enreg 2 /*nombre de fichiers on peut représenter ses métadonnées dans un seule enregistrement.
ça veut dire on peut representer des métadonnées de 2 fichiers dans un seule enregistrement de bloc réservé pour les métadonnées.*/


//déclaration des structures nécessaires:


typedef struct enregistrement{
    
    int id;
    
    char data[data_size];

    bool isDeleted; //ce variable est utilisé pour la suppression logique des enregistrements.

}enregistrement;


typedef struct Bloc{
    
    enregistrement enregistrements[FB];

    int nbrE; //le nombre d'enregistrements actuellement occupé dans un Bloc

    int nextBloc; /*un entier qui conserve l'index de prochaine Bloc dans le cas
    d'organisation chainée*/

}Bloc, Buffer; //le buffer contient la meme structure que un Bloc.

typedef struct Memoire_Secondaire{

    FILE *memoireS;

    int nbrBlocs; //cette variable indique le nombre de Blocs maximale de la memoire secrondaire.

    int bloc_size; //nombre d'enregistrements maximale dans un bloc

    double TailleMemoire;

}MS;

typedef struct Meta_Donnes{

    char nom[50];

    int taille_Blocs; //il prend 4 charachtéres dans data[data_size] d'un l'enregistrement.

    int taille_Enrgs; //+4

    int adresse_PremierBloc; //+4

    int OrganisationE; //on représent L'Organisation Contigue par 1 et Chainée par 0.

    int OrganisationI; //on représente L'Organisation Ordonné par 1 et NonOrdonné par 0


}MetaD;


typedef struct produit{ //exemplaire a revoir.
    char nom[27];

    double prix;

    int id;
}produit;


//LES FUNCTIONS:

void InitialiseDisk(MS* MemoireS){/* Cette fonction est utilisée lorsqu'on réinitialise la mémoire secondaire, et après avoir effectué
                                les opérations nécessaires, on sauvegarde l'état et les données de la mémoire secondaire dans un fichier binaire.
                                Si le fichier binaire de la mémoire secondaire existe, on n'utilise pas cette fonction ; on lit simplement
                                le fichier binaire et alloue un espace mémoire pour la mémoire secrondaire sauvegardée dans le fichier.
                                */

    MemoireS->bloc_size=FB;

    MemoireS->nbrBlocs = NbrBlocs;

    MemoireS->memoireS = fopen("C:\\Users\\ACER\\Desktop\\Projet SFSD\\memoire_secondaire.bin","wb+");

    if(MemoireS->memoireS == NULL){
        printf("\nErreur lors la initialisation du disk.");
        return;
    }
    

    int nbr_MetaBlocs = ceil( (NbrBlocs - 1) / ( FB * nbrMeta_enreg ) );/* Le nombre maximal de blocs de métadonnées pour chaque fichier est une majoration du nombre maximal de blocs
    qui peuvent être occupés par un fichier, divisé par le facteur de blocage. On divise par le facteur de blocage
    car chaque enregistrement d'un bloc contient des métadonnées d'un fichier. */


//initialisation de la table d'allocation dans Bloc 0 :

    Bloc bloc0;

    bloc0.nbrE = 0;

int i,j=0,k=0,nbrBlocs_Libre=0;

    for(i=0; i<NbrBlocs; i++){

        if(j >= data_size){ /*cette condition est utilisé si par exemple on change le nombres de Blocs définé dans la Memoire Secrondaire
                            tellque il devient plus que le data_size, dans ce cas il faut que a chaque fois le tableau de charactéres (data)
                            de l'enregistrement k est plein, on va vers le prochaine tableau de charactéres dans l'enregistrement k+1 et continuée
                            
                            NOTE : le tableau de charachtéres (data) dans le Bloc 0 est représenté come une table d'allocation!*/
            k++;

            j=0;
        }

        if(i <= nbr_MetaBlocs){/*
            on initialise la table d'allocation tellque data[0] = 1 represente que le Bloc 0 est occupé,
            data[1] = 1 represente que le Bloc 1 est Occupé etc...,data[nbr_MetaBlocs +1] = 0 represente
            que Le Bloc nbr_MetaBlocs + 1 est libre...etc jusqu'a data[NbrBlocs].
            (MAIS PAS EXCATEMENT A data[NbrBlocs] Car le tableau d'enregistrement 0
            contient 140 charachtéres et donc on va vers prochaine enregistrement!)
            */
        bloc0.enregistrements[k].data[j] = 1;



        }else{
        
        bloc0.enregistrements[k].data[j] = 0;

        nbrBlocs_Libre++;

        }
        j++;
    }

    bloc0.nbrE=k+1; //nombres d'enregistrements occupé qui contient le contenu de la table d'allocation dans Bloc 0.

    bloc0.nextBloc = nbrBlocs_Libre; /* comme Bloc 0 est utilisé pour sauvgarder la table d'allocation de la Memoire Secrondaire,
                                                  la variable NextBloc ne sera pas étres utilisé, de plus le Bloc 0 ne sera pas étres concerné
                                                  par les opérations come le compactage ou défragmentation, c'est un Bloc presque isolé, et alors 
                                                  on a utilisé NextBloc come le nombres de blocs libres dans la table d'allocation.
                                                  */
    
    fwrite(&bloc0,sizeof(Bloc),1,MemoireS->memoireS); //sauvgarder le Bloc 0 dans la memoire secondaire aprés l'initialisation de la table d'allocation.


    //initialisation des autres blocs (de Bloc 1 vers Bloc 255):

    Bloc bloc;

    bloc.nbrE = 0;

    bloc.nextBloc =-1; /* le -1 représente que le next bloc n'existe pas.*/

    for(i=0; i<FB; i++){ /*initialiser les tableau de charactéres de chaque enregistrements par null '/0' */
    
    memset(bloc.enregistrements[i].data, 0, sizeof(bloc.enregistrements[i].data));

    }


    for(i=1; i<NbrBlocs; i++){
    
    fwrite(&bloc,sizeof(bloc),1,MemoireS->memoireS);

    }
    
    fclose(MemoireS->memoireS);
    printf("\nInitialisation du disk avec succés.");

}





MetaD CreeFichier(){

MetaD fichierinfo;

    printf("\nLe nom du fichier: ");

    fgets(fichierinfo.nom,sizeof(fichierinfo.nom),stdin);
    
    do{

    printf("\nNombre d'Enregistrements: ");

    scanf("%d",&fichierinfo.taille_Enrgs);

    }while(fichierinfo.taille_Enrgs<=0);

    do{

    printf("\nMode d'Organisation Externe:\n1: ORGANISATION CONTIGUE\t0: ORGANISATION CHAINEE\nEntrer: ");
    
    scanf("%d",&fichierinfo.OrganisationE);

    }while(fichierinfo.OrganisationE != 1 && fichierinfo.OrganisationE != 0);

    do{

    printf("\nMode d'Organisation Interne:\n1: ORGANISATION ORDONNE\t0: ORGANISATION NON-ORDONNE\nEntrer: ");

        scanf("%d",&fichierinfo.OrganisationI);

    }while(fichierinfo.OrganisationI != 1 && fichierinfo.OrganisationI != 0);

    fichierinfo.taille_Blocs = fichierinfo.taille_Enrgs / FB; // le nombres de blocs nécessaire pour crée le fichier dans la memoire secrondaire.

    if(fichierinfo.taille_Enrgs % FB != 0){

        fichierinfo.taille_Blocs += 1;/*si par exemple le nombre d'enregistrements d'un fichier n'est pas un multiple de FB par exemple 13 enregistrements,
                        alors le nombres de Bloc nécessaire pour stocker le fichier est 2 Blocs pour 12 enregistrements + 1 Bloc pour le reste.*/

    }

    return fichierinfo;

}

bool GES_Creation(MetaD *fichierinfo,Bloc *Bloc0){


if(Bloc0->nextBloc >= fichierinfo->taille_Blocs){

    int i=0,j=0,k=0;

    if(fichierinfo->OrganisationE == true){ //si l'Organisation est contigue

        int counter=0;

        while(i<NbrBlocs && counter != fichierinfo->taille_Blocs){

            if(j>data_size){
                k++;
                j=0;
            }
        
        if(Bloc0->enregistrements[k].data[j] == 0){

            counter++;

        }else{//on chereche n blocs contigue dans la memoire secrondaire ou n = nbrBlocs

            counter=0;

        }

        j++;

        i++;
            
        }
        if(counter == fichierinfo->taille_Blocs){

            if(k == 0){

            fichierinfo->adresse_PremierBloc = j - counter; //le index est l'adresse du Bloc de départ du fichier qui sera étre allouer dans la mémoire secrondaire.

            }else{

            fichierinfo->adresse_PremierBloc = k *(data_size - 1) + j + 1 - counter;
            }

            return true;

        }else{

            //pas d'espace contigue ---> compactage , pour le moment on fait return false.

            

            fichierinfo->adresse_PremierBloc = -1;

            return false;
        }


    }else{
            //Organisation Chainé, dans cette organisation il faut juste de trouver le premier bloc libre.

            bool trouver = false;

            while(i<NbrBlocs && trouver == false){

            if(j>data_size){

                k++;

                j=0;

            }

            if(Bloc0->enregistrements[k].data[j] == 0){

                trouver = true;

                if(k == 0){

                    fichierinfo->adresse_PremierBloc = j;

                }else{
                
                fichierinfo->adresse_PremierBloc = k*(data_size - 1) + j + 1;

                }
            }
            
            j++;
            i++;
    }
    
        return trouver; // pas de compactage si on peut pas trouver un Bloc libre dans l'organisation Chainé.
    }

}else{

    //nombres de bloc libres insuffisant pour ajouter le fichier (dans les deux modes!) ---> pas d'espace memoire!.

    printf("\nLa Memoire Secrondaire est pleine!");

    fichierinfo->adresse_PremierBloc = -1;

    return false;
}

}




void EcrireMetaD_Enreg(Bloc *MetaBloc, MetaD fichierinfo, int niveau,int adress) { 
/*
cette fonction est la base d'écriture de MetaDonné d'un fichier dans
un bloc de MetaDonne, puisque chaque enregistrement contient un tableau de 140 charactérs, 
et notre metadonné reserve un espace equivalent de 70 charactérs(voire la structure de métadonné) 
alors on peut sauvgardé métadonné de deux fichier niveau 1 et niveau 2 telleque :
(Enregistrement[0].data de 0 a 70 == niveau 1 et Enregistrement[0].data de 70 a 140 == niveau 2)
*/

    if(niveau == 1){

    memcpy(MetaBloc->enregistrements[adress].data, fichierinfo.nom, 50); 

    memcpy(MetaBloc->enregistrements[adress].data + 50, &fichierinfo.taille_Blocs, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 54, &fichierinfo.taille_Enrgs, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 58, &fichierinfo.adresse_PremierBloc, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 62, &fichierinfo.OrganisationE, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 66, &fichierinfo.OrganisationI, sizeof(int));

        }else{

        if(niveau == 2){

    memcpy(MetaBloc->enregistrements[adress].data +70, fichierinfo.nom, 50);
    memcpy(MetaBloc->enregistrements[adress].data + +120, &fichierinfo.taille_Blocs, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 124, &fichierinfo.taille_Enrgs, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 128, &fichierinfo.adresse_PremierBloc, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 132, &fichierinfo.OrganisationE, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 136, &fichierinfo.OrganisationI, sizeof(int));

        }else{
            printf("\nErreur!, dans function *EcrireMetaD_Enreg*, niveau saisie n'existe pas!.");
            return;
        }
        }


}



MetaD LireMetaD_Enreg(Buffer MetaBloc,int niveau , int adress){

    /*
    Cette fonction fait l'inverse de EcrireMetaD_Enreg. Elle lit la métadonnée d'un fichier dans un bloc de métadonnée avec l'adresse et le niveau
    correspondant.
    */

    
    MetaD fichierinfo;

        if(niveau == 1){

        
        memcpy(fichierinfo.nom, MetaBloc.enregistrements[adress].data, 50);

        memcpy(&fichierinfo.taille_Blocs, MetaBloc.enregistrements[adress].data + 50, sizeof(int));

        memcpy(&fichierinfo.taille_Enrgs, MetaBloc.enregistrements[adress].data + 54, sizeof(int));

        memcpy(&fichierinfo.adresse_PremierBloc, MetaBloc.enregistrements[adress].data + 58, sizeof(int));

        memcpy(&fichierinfo.OrganisationE, MetaBloc.enregistrements[adress].data + 62, sizeof(int));

        memcpy(&fichierinfo.OrganisationI, MetaBloc.enregistrements[adress].data + 66,sizeof(int));

        }else{

            if(niveau == 2){
        
        memcpy(fichierinfo.nom, MetaBloc.enregistrements[adress].data + 70 , 50);

        memcpy(&fichierinfo.taille_Blocs, MetaBloc.enregistrements[adress].data + 120, sizeof(int));

        memcpy(&fichierinfo.taille_Enrgs, MetaBloc.enregistrements[adress].data + 124, sizeof(int));

        memcpy(&fichierinfo.adresse_PremierBloc, MetaBloc.enregistrements[adress].data + 128, sizeof(int));

        memcpy(&fichierinfo.OrganisationE, MetaBloc.enregistrements[adress].data + 132, sizeof(int));

        memcpy(&fichierinfo.OrganisationI, MetaBloc.enregistrements[adress].data + 136,sizeof(int));

        }else{
            printf("\nErreur dans function *LireMetaD_Enreg*, niveau saise n'existe pas!");
            return fichierinfo;
        }
        }
        

        return fichierinfo;

}


void sauvgarder_MetaD(MS MemoireS, MetaD fichierinfo,Buffer MetaBloc){ /*cette function sauvgarde les métadonnés d'un fichier dans la memoire secondaire*/
    
    rewind(MemoireS.memoireS);

    bool trouver = false;

    int bloc_index = 1;

    fseek(MemoireS.memoireS,sizeof(Bloc),SEEK_CUR);

    while(trouver == false && bloc_index <= ceil( (NbrBlocs - 1) / ( FB * nbrMeta_enreg ) )){

        fread(&MetaBloc,sizeof(Buffer),1,MemoireS.memoireS);

        if(MetaBloc.nbrE < FB){ // on vérfifier est-ce-que les enregistrements de bloc meta donnes n'est pas pleine!.

        trouver = true;

        if(MetaBloc.enregistrements[MetaBloc.nbrE].data[0] == '\0'){ //on vérifier est-ce-que niveau 1 de l'enregistrement contient déja des information de metadonné d'un seule fichier ou non.

        EcrireMetaD_Enreg(&MetaBloc,fichierinfo,1,MetaBloc.nbrE);

        }else{

        EcrireMetaD_Enreg(&MetaBloc,fichierinfo,2,MetaBloc.nbrE);

        
        MetaBloc.nbrE++;
    /*
    Dans ce cas, l'enregistrement de l'adresse MetaBloc.nbrE devient plein ! (car les niveaux 1 et 2 deviennent pleins) alors on fait MetaBloc.nbrE++; .
    NOTE IMPORTANTE : Le nbrE dans les blocs réservés comme MetaBlocs représente le nombre d'enregistrements PLEINS et non le nombre d'enregistrements OCCUPÉS.
    */


        }

    }else{

        bloc_index++;

    }
    }

    if(trouver == true){
        
        fseek(MemoireS.memoireS, bloc_index * sizeof(Bloc),SEEK_SET);

        fwrite(&MetaBloc,sizeof(Buffer),1,MemoireS.memoireS);

        printf("\nMetaData de fichier %s sauvgarder avec sucess!. dans %d",fichierinfo.nom,MetaBloc.nbrE);

        
        



    }else{
        printf("\nMetaData de fichier %s ne peut pas étre sauvgarder.",fichierinfo.nom);
    }

    

}

void AllouerFichier(MS MemoireS,MetaD fichierinfo){
    
    Bloc* Buffer = (Bloc *)malloc(fichierinfo.taille_Blocs * sizeof(Bloc));

    int i,nbrEReste=fichierinfo.taille_Enrgs;

    for(i=0; i<fichierinfo.taille_Blocs; i++){

        if(nbrEReste/FB >0){

        Buffer[i].nbrE = FB;

    }else{

        Buffer[i].nbrE = nbrEReste;

    }

    nbrEReste -= FB;
    
    }

    Bloc Bloc0;

    rewind(MemoireS.memoireS);

    fread(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

    int k = fichierinfo.adresse_PremierBloc/data_size; //calculer dans quelle enregistrement l'état de PremierBloc dans la table d'allocation est situé.

    int j = fichierinfo.adresse_PremierBloc - k * data_size;

    if(fichierinfo.OrganisationE == 1){ //organisation contigue

    for(i=0; i<fichierinfo.taille_Blocs; i++){

            if(j>=data_size){
                k +=1;
                j =0;
            }

            Bloc0.enregistrements[k].data[j] = 1;

            fwrite(&Buffer[i],sizeof(Bloc),1,MemoireS.memoireS);

            j +=1;

    }
    }else{
        if(fichierinfo.OrganisationE == 0){ //Organisation Chainé
        
        int i=0;


        while(i<fichierinfo.taille_Blocs){

            if(j>=data_size){
                k +=1;
                j =0;
            }
        if(Bloc0.enregistrements[k].data[j] == 0){
        
        Bloc0.enregistrements[k].data[j] = 1;

        i++;

        if(i>0){

            Buffer[i-1].nextBloc = k*data_size + j; //on remplie le variable de chainage des Blocs dans le Buffer.

        }

        }

        j +=1;



        }
        
        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

        i=0;

        while(i<fichierinfo.taille_Blocs){

        fwrite(&Buffer[i],sizeof(Bloc),1,MemoireS.memoireS);
        
        fseek(MemoireS.memoireS,Buffer[i].nextBloc*sizeof(Bloc),SEEK_SET);

        i++;

        }

        }
    }
    
    rewind(MemoireS.memoireS);

    fwrite(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS); //mise a jour de table d'allocation.

    free(Buffer);
}

int main(){


MS MemoireS;

Buffer Bloc0,MetaBloc,TestBloc;

MetaD fichier,fichierTest,fichier2,fichierTest2,fichierTest3,fichier3;

bool etat;

InitialiseDisk(&MemoireS);

MemoireS.memoireS = fopen("C:\\Users\\ACER\\Desktop\\Projet SFSD\\memoire_secondaire.bin","rb+");

if(MemoireS.memoireS == NULL){
    printf("\nError!");
}else{


fichier = CreeFichier();

getchar();

fichier2 = CreeFichier();


rewind(MemoireS.memoireS);

fread(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

etat = GES_Creation(&fichier,&Bloc0);

printf("\nGES_Creation : %d , Adress Premier Bloc: %d",etat,fichier.adresse_PremierBloc);

AllouerFichier(MemoireS,fichier);

rewind(MemoireS.memoireS);

fread(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

etat = GES_Creation(&fichier2,&Bloc0);

printf("\nGES_Creation : %d , Adress Premier Bloc: %d",etat,fichier2.adresse_PremierBloc);

AllouerFichier(MemoireS,fichier2);

sauvgarder_MetaD(MemoireS,fichier,MetaBloc);

sauvgarder_MetaD(MemoireS,fichier2,MetaBloc);


rewind(MemoireS.memoireS);

fseek(MemoireS.memoireS,sizeof(Bloc),SEEK_SET);

fread(&TestBloc,sizeof(Bloc),1,MemoireS.memoireS);

    fichierTest = LireMetaD_Enreg(TestBloc,1,0);

    fichierTest2 = LireMetaD_Enreg(TestBloc,2,0);
 

    printf("\nFichierTest nom : %s\ntaille_Blocs = %d\nTaile_Enrgs=%d\nAdresse_Premier_Bloc=%d\nOrganisationE=%d\nOrganisationI=%d",fichierTest.nom,fichierTest.taille_Blocs,fichierTest.taille_Enrgs,fichierTest.adresse_PremierBloc,fichierTest.OrganisationE,fichierTest.OrganisationI);
    printf("\nFichierTest nom : %s\ntaille_Blocs = %d\nTaile_Enrgs=%d\nAdresse_Premier_Bloc=%d\nOrganisationE=%d\nOrganisationI=%d",fichierTest2.nom,fichierTest2.taille_Blocs,fichierTest2.taille_Enrgs,fichierTest2.adresse_PremierBloc,fichierTest2.OrganisationE,fichierTest2.OrganisationI);



    rewind(MemoireS.memoireS);

    Bloc bloc1;

int i=0,j=0,k=0;;

fread(&bloc1,sizeof(bloc1),1,MemoireS.memoireS);

    while(i<NbrBlocs){
        if(j>=data_size){
            k++;
            j=0;
        }
        
        printf("\nBloc %d : state = %d",i,bloc1.enregistrements[k].data[j]);

        j++;
        i++;
    
    }

    i=0;

    rewind(MemoireS.memoireS);


    while(fread(&bloc1,sizeof(bloc1),1,MemoireS.memoireS) != 0){
        
        printf("\nBloc %d : NbrE= %d NextBloc = %d",i,bloc1.nbrE,bloc1.nextBloc);
        i++;
    
    }


}



}

