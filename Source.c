#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#pragma warning(disable : 4996)

// declaration des variables
enum
{
    NB_Semestre = 2,
    MIN_UE = 3,
    MAX_UE = 6,
    NB_Matiere = 4,
    NB_Epreuves = 8,
    NB_Etudiants = 180,
    NB_CHAR = 30,
};

const float MIN_NOTE = 0.f, MAX_NOTE = 20.f;
typedef char CH30[CHAR_MAX + 1];

typedef struct
{
    CH30 nom;
    float notes[NB_Semestre * NB_Matiere * NB_Epreuves];
} Etudiant;

typedef struct
{
    CH30 nom;
    float coef[MAX_UE];
} Epreuve;

typedef struct
{
    CH30 nom;
    unsigned int nbEpreuves;
    Epreuve epreuves[NB_Epreuves];
} Matiere;

typedef struct
{
    unsigned int nbMatiere;
    Matiere matiere[NB_Matiere];
} Semestre;

typedef struct
{
    unsigned int nbUE; // nombre de coef, commun a toutes les epreuves
    Semestre semestres[NB_Semestre];
    unsigned int nbEtudiants;
    Etudiant etudiants[NB_Etudiants];
} Formation;

// Decalrations des fonctions
void initialisation(Formation *F);
int indexMatiere(Semestre *S, CH30 n);
int indexEpreuve(Matiere *M, CH30 n);
int indexEtudiant(Formation *F, CH30 n);
int coeffIncorrects(Semestre *S, int nbUE);
int notesIncorrectes(Formation *F, Etudiant *E, int s);
void moyenneUEMat(Matiere *M, Etudiant *E, int iUE, int s, int iMat, float *retour);
float moyenneUE(Semestre *S, Etudiant *E, int iUE, int iSem);
void formation(Formation *F);
void epreuve(Formation *F);
void coefficients(Formation *F);
void note(Formation *F);
void notes(Formation *F);
void releve(Formation *F);
void decision(Formation *F);

int main()
{
    setlocale(LC_ALL, "");
    char cde[NB_CHAR] = "";
    Formation Form;
    initialisation(&Form);
    do
    {
        if (scanf("%s", &cde) != 0)
        {
            if (strcmp(cde, "formation") == 0)
                formation(&Form); // C2
            else if (strcmp(cde, "epreuve") == 0)
                epreuve(&Form); // C3
            else if (strcmp(cde, "coefficients") == 0)
                coefficients(&Form); // C4
            else if (strcmp(cde, "note") == 0)
                note(&Form); // C5
            else if (strcmp(cde, "notes") == 0)
                notes(&Form); // C6
            else if (strcmp(cde, "releve") == 0)
                releve(&Form); // C7
            else if (strcmp(cde, "decision") == 0)
                decision(&Form); // C8
        }
    } while (strcmp(cde, "exit") != 0); // C1
    return 0;
}

// Les fonctions pour les questions
void initialisation(Formation *F)
{
    F->nbUE = 0;
    for (int i = 0; i < NB_Semestre; i++)
    {
        F->semestres[i].nbMatiere = 0;
        for (int j = 0; j < NB_Matiere; j++)
        {
            F->semestres[i].matiere[j].nbEpreuves = 0;
        }
    }
    F->nbEtudiants = 0;
    for (int i = 0; i < NB_Etudiants; i++)
    {
        for (int j = 0; j < NB_Semestre * NB_Matiere * NB_Epreuves; j++)
        {
            F->etudiants[i].notes[j] = -1.f;
        }
    }
}

//
int indexMatiere(Semestre *S, CH30 n)
{
    for (int i = 0; i < S->nbMatiere; i++)
        if (strcmp(n, S->matiere[i].nom) == 0)
            return i;
    return -1;
}

// Commentaire
int indexEpreuve(Matiere *M, CH30 n)
{
    for (int i = 0; i < M->nbEpreuves; i++)
        if (strcmp(n, M->epreuves[i].nom) == 0)
            return i;
    return -1;
}
// Commentaire
int indexEtudiant(Formation *F, CH30 n)
{
    for (int i = 0; i < F->nbEtudiants; i++)
        if (strcmp(n, F->etudiants[i].nom) == 0)
            return i;
    return -1;
}

// Verifier si les coefficients sont corrects
int coeffIncorrects(Semestre *S, int nbUE)
{
    float sommeCoeff[MAX_UE]; // on doit v�rifier la pr�sence de coefficients por chaque UE;
    for (int i = 0; i < MAX_UE; i++)
        sommeCoeff[i] = 0.f;
    for (int i = 0; i < S->nbMatiere; i++)
        for (int j = 0; j < S->matiere[i].nbEpreuves; j++)
            for (int k = 0; k < nbUE; k++)
                sommeCoeff[k] += S->matiere[i].epreuves[j].coef[k];
    for (int i = 0; i < nbUE; i++)
        if (sommeCoeff[i] == 0.f)
            return 1;
    return 0;
}

// Verifier si les notes sont corrects
int notesIncorrectes(Formation *F, Etudiant *E, int s)
{
    for (int i = 0; i < F->semestres[s].nbMatiere; i++)
        for (int j = 0; j < F->semestres[s].matiere[i].nbEpreuves; j++)
            if (E->notes[(s * NB_Matiere * NB_Epreuves) + (i * NB_Epreuves) + j] < MIN_NOTE)
                return 1;
    return 0;
}

void moyenneUEMat(Matiere *M, Etudiant *E, int iUE, int s, int iMat, float *retour)
{
    float sommeCoef = 0;
    float result = 0.f;
    for (int i = 0; i < M->nbEpreuves; i++)
    {
        result += (E->notes[(s * NB_Matiere * NB_Epreuves) + (iMat * NB_Epreuves) + i]) * M->epreuves[i].coef[iUE];
        sommeCoef += M->epreuves[i].coef[iUE];
    }
    retour[0] = sommeCoef;
    retour[1] = result;
}

float moyenneUE(Semestre *S, Etudiant *E, int iUE, int iSem)
{
    float sommeCoef = 0.f;
    float result = 0.f;
    for (int i = 0; i < S->nbMatiere; i++)
    {
        float retour[2];
        moyenneUEMat(&S->matiere[i], E, iUE, iSem, i, retour);
        result += retour[1];
        sommeCoef += retour[0];
    }
    return result / sommeCoef;
}

// C2
void formation(Formation *F)
{
    int ue;
    scanf("%d", &ue);
    if (F->nbUE != 0)
    {
        printf("Le nombre d'UE est deja defini\n");
        return;
    }
    if (ue >= MIN_UE && ue <= MAX_UE)
    {
        printf("Le nombre d'UE est defini\n");
        F->nbUE = ue;
    }
    else
        printf("Le nombre d'UE est incorrect\n");
}

// C3
void epreuve(Formation *F)
{
    int x;
    scanf("%d", &x);
    if (x > NB_Semestre || x <= 0) // c'est pour arreter le programme quand la valeur de semestre est incorrect
    {
        printf("Le num�ro de semestre est incorrect\n");
        return;
    }
    x--;

    CH30 NomMat; // declaration de Nom de Matiere
    scanf("%s", &NomMat);

    CH30 NomEpr;
    scanf("%s", &NomEpr);

    float coef[MAX_UE];
    unsigned int nbZero = 0;
    for (int i = 0; i < F->nbUE; i++)
    {
        scanf("%f", &coef[i]);
        if (coef[i] == 0.f)
            nbZero++;
    }
    if (nbZero == F->nbUE)
    {
        printf("Au moins un des coefficients est incorrect\n");
        return;
    }

    int indexM = indexMatiere(&F->semestres[x], NomMat);

    if (indexM < 0)
    {
        indexM = F->semestres[x].nbMatiere;
        F->semestres[x].nbMatiere++;
        strcpy(F->semestres[x].matiere[indexM].nom, NomMat);
        printf("Matiere ajoutee a la formation\n");
    }

    int indexE = indexEpreuve(&F->semestres[x].matiere[indexM], NomEpr);

    if (indexE >= 0)
    {
        printf("Une meme epreuve existe deja\n");
        return;
    }

    indexE = F->semestres[x].matiere[indexM].nbEpreuves;
    F->semestres[x].matiere[indexM].nbEpreuves++;
    strcpy(F->semestres[x].matiere[indexM].epreuves[indexE].nom, NomEpr);
    printf("Epreuve ajoutee a la formation\n");

    for (int i = 0; i < F->nbUE; i++)
        F->semestres[x].matiere[indexM].epreuves[indexE].coef[i] = coef[i];
}

// C4
void coefficients(Formation *F)
{
    int s;
    scanf("%d", &s);
    if (s > NB_Semestre || s <= 0)
    {
        printf("Le numero de semestre est incorrect\n");
        return;
    }
    s--;

    int nbEpr = 0; // on recherche le nombre d'�preuves pour v�rifier s'il y en a;
    for (int i = 0; i < F->semestres[s].nbMatiere; i++)
    {
        nbEpr += F->semestres[s].matiere[i].nbEpreuves;
    }
    if (nbEpr == 0)
    {
        printf("Le semestre ne contient aucune epreuve\n");
        return;
    }

    if (coeffIncorrects(&F->semestres[s], F->nbUE))
    {
        printf("Les coefficients d'au moins une UE de ce semestre sont tous nuls\n");
        return;
    }

    printf("Coefficients corrects\n");
}

// C5
void note(Formation *F)
{
    int s;
    scanf("%d", &s);
    if (s > NB_Semestre || s <= 0) // c'est pour arreter le programme quand la valeur de semestre est incorrect
    {
        printf("Le num�ro de semestre est incorrect\n");
        return;
    }
    s--;

    CH30 NomEtu;
    scanf("%s", &NomEtu);

    CH30 NomMat;
    scanf("%s", &NomMat);
    int iMat = indexMatiere(&F->semestres[s], NomMat);
    if (iMat < 0)
    {
        printf("Matiere inconnue\n");
        return;
    }

    CH30 NomEpr;
    scanf("%s", &NomEpr);
    int iEpr = indexEpreuve(&F->semestres[s].matiere[iMat], NomEpr);
    if (iEpr < 0)
    {
        printf("Epreuve inconnue\n");
        return;
    }

    float noteEtu;
    scanf("%f", &noteEtu);
    if (noteEtu > MAX_NOTE || noteEtu < MIN_NOTE) // c'est pour arreter le programme quand la note est incorrecte
    {
        printf("Note incorrecte\n");
        return;
    }

    int iEtu = indexEtudiant(F, NomEtu); // On met F et non &F car F est d�j� un pointeur via les param�tres de la fonction note
    if (iEtu < 0)
    {
        iEtu = F->nbEtudiants;
        F->nbEtudiants++;
        strcpy(F->etudiants[iEtu].nom, NomEtu);
        printf("Etudiant ajoute a la formation\n");
    }

    if (F->etudiants[iEtu].notes[(s * NB_Matiere * NB_Epreuves) + (iMat * NB_Epreuves) + iEpr] >= MIN_NOTE)
    {
        printf("Une note est deja definie pour cet etudiant\n");
        return;
    }

    F->etudiants[iEtu].notes[(s * NB_Matiere * NB_Epreuves) + (iMat * NB_Epreuves) + iEpr] = noteEtu;
    printf("Note ajoutee a l'etudiant\n");
}

// C6
void notes(Formation *F)
{
    int s;
    scanf("%d", &s);
    if (s > NB_Semestre || s <= 0) // c'est pour arreter le programme quand la valeur de semestre est incorrect
    {
        printf("Le num�ro de semestre est incorrect\n");
        return;
    }
    s--;

    CH30 NomEtu;
    scanf("%s", &NomEtu);
    int iEtu = indexEtudiant(F, NomEtu); // On met F et non &F car F est d�j� un pointeur via les param�tres de la fonction note
    if (iEtu < 0)
    {
        printf("Etudiant inconnu\n");
        return;
    }

    if (notesIncorrectes(F, &F->etudiants[iEtu], s))
    {
        printf("Il manque au moins une note pour cet etudiant\n");
        return;
    }

    printf("Notes correctes\n");
}

// C7
void releve(Formation *F)
{
    int s;
    scanf("%d", &s);
    if (s > NB_Semestre || s <= 0) // c'est pour arreter le programme quand la valeur de semestre est incorrect
    {
        printf("Le num�ro de semestre est incorrect\n");
        return;
    }
    s--;

    CH30 NomEtu;
    scanf("%s", &NomEtu);
    int iEtu = indexEtudiant(F, NomEtu); // On met F et non &F car F est d�j� un pointeur via les param�tres de la fonction note
    if (iEtu < 0)
    {
        printf("Etudiant inconnu\n");
        return;
    }

    if (coeffIncorrects(&F->semestres[s], F->nbUE))
    {
        printf("Les coefficients de ce semestre sont incorrects\n");
        return;
    }

    if (notesIncorrectes(F, &F->etudiants[iEtu], s))
    {
        printf("Il manque au moins une note pour cet �tudiant\n");
        return;
    }

    int maxMot = strlen("Moyennes");
    for (int i = 0; i < F->semestres[s].nbMatiere; i++)
        if (strlen(F->semestres[s].matiere[i].nom) > maxMot)
            maxMot = strlen(F->semestres[s].matiere[i].nom);
    for (int i = 0; i < maxMot; i++)
        printf(" ");
    for (int i = 0; i < F->nbUE; i++)
        printf("  UE%d", i + 1);
    for (int i = 0; i < F->semestres[s].nbMatiere; i++)
    {
        printf("\n%s", F->semestres[s].matiere[i].nom);
        for (int j = 0; j < maxMot - strlen(F->semestres[s].matiere[i].nom); j++)
            printf(" ");
        for (int j = 0; j < F->nbUE; j++)
        {
            float results[2];
            moyenneUEMat(&F->semestres[s].matiere[i], &F->etudiants[iEtu], j, s, i, results);
            if (results[0] == 0.f)
                printf("   ND");
            else
            {
                float result = results[1] / results[0];
                if (result < 10.f)
                    printf("  %.1f", floorf(10.0 * result) / 10.0);
                else
                    printf(" %.1f", floorf(10.0 * result) / 10.0);
            }
        }
    }
    printf("\n--\nMoyennes");
    for (int i = 0; i < maxMot - strlen("Moyennes"); i++)
        printf(" ");
    for (int i = 0; i < F->nbUE; i++)
    {
        float moy = moyenneUE(&F->semestres[s], &F->etudiants[iEtu], i, s);
        if (moy < 10.f)
            printf("  %.1f", floorf(10.0 * moy) / 10.0);
        else
            printf(" %.1f", floorf(10.0 * moy) / 10.0);
    }
    printf("\n");
}

// C8
void decision(Formation *F)
{
    CH30 NomEtu;
    scanf("%s", &NomEtu);
    int iEtu = indexEtudiant(F, NomEtu); // On met F et non &F car F est deja un pointeur via les parametres de la fonction note
    if (iEtu < 0)
    {
        printf("Etudiant inconnu\n");
        return;
    }

    for (int s = 0; s < NB_Semestre; s++)
        if (coeffIncorrects(&F->semestres[s], F->nbUE))
        {
            printf("Les coefficients d'au moins un semestre sont incorrects\n");
            return;
        }

    for (int s = 0; s < NB_Semestre; s++)
        if (notesIncorrectes(F, &F->etudiants[iEtu], s))
        {
            printf("Il manque au moins une note pour cet �tudiant\n");
            return;
        }

    int maxMot = strlen("Moyennes annuelles");
    for (int i = 0; i < maxMot; i++)
        printf(" ");
    for (int i = 0; i < F->nbUE; i++)
        printf("  UE%d", i + 1);
    for (int s = 0; s < NB_Semestre; s++)
    {
        printf("\nS%d", s + 1);
        for (int i = 0; i < maxMot - 2; i++)
            printf(" ");
        for (int i = 0; i < F->nbUE; i++)
        {
            float moy = moyenneUE(&F->semestres[s], &F->etudiants[iEtu], i, s);
            if (moy < 10.f)
                printf("  %.1f", floorf(10.0 * moy) / 10.0);
            else
                printf(" %.1f", floorf(10.0 * moy) / 10.0);
        }
    }
    printf("\n--\nMoyennes annuelles");
    int nbUEV = 0; // On compte le nombre d'UE valid�es
    float moyUE[MAX_UE];
    for (int i = 0; i < F->nbUE; i++)
    {
        moyUE[i] = 0.f;
        for (int s = 0; s < NB_Semestre; s++)
            moyUE[i] += moyenneUE(&F->semestres[s], &F->etudiants[iEtu], i, s);
        moyUE[i] = moyUE[i] / 2.0;
        if (moyUE[i] < 10.f)
            printf("  %.1f", floorf(10.0 * moyUE[i]) / 10.0);
        else
        {
            nbUEV++;
            printf(" %.1f", floorf(10.0 * moyUE[i]) / 10.0);
        }
    }
    printf("\nAcquisition");
    for (int i = 0; i < maxMot - strlen("Acquisition"); i++)
        printf(" ");
    if (nbUEV == 0)
        printf("Aucune");
    else
    {
        int nbUEAff = 0; // On compte le nbre d'UE affch�es pour mettre en place la syntaxe
        for (int i = 0; i < F->nbUE; i++)
            if (moyUE[i] >= 10.f)
            {
                if (nbUEAff == 0)
                {
                    nbUEAff++;
                    printf(" UE%d", i + 1);
                }
                else if (nbUEAff == nbUEV - 1)
                {
                    printf(" et UE%d", i + 1);
                    break;
                }
                else
                {
                    printf(", UE%d", i + 1);
                    nbUEAff++;
                }
            }
    }
    printf("\nDevenir");
    for (int i = 0; i < maxMot - strlen("Devenir"); i++)
        printf(" ");
    if (nbUEV > F->nbUE / 2)
        printf(" Passage");
    else
        printf(" Redoublement");
    printf("\n");
}