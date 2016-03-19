//Les structures PORTD et compagnie sont la pour m'éviter les erreurs CLion, il faut les retirer dans le programme final
//Il manque également les fonctions du squelette et l'init, qui a normalement été fait par Samuel

struct PORTD
{
	char RD0;
	char RD1;
	char RD2;
	char RD3;
	char RD4;
	char RD5;
	char RD6;
	char RD7;
} PORTDbits;

struct PORTC
{
	char RC0;
	char RC1;
	char RC2;
	char RC3;
	char RC4;
	char RC5;
	char RC6;
	char RC7;
} PORTCbits;

struct PORTB
{
	char RB0;
	char RB1;
	char RB2;
	char RB3;
	char RB4;
	char RB5;
	char RB6;
	char RB7;
} PORTBbits;

char PORTB, PORTC, PORTD;

#define PERIODE_AFF 200
#define MULTI_FAIBLE PORTDbits.RD3
#define MULTI_FORT PORTDbits.RD4
#define AFFICHEUR PORTB
#define LED_HAUT PORTDbits.RD1
#define LED_BAS PORTDbits.RD2
#define INTERR_ALARME PORTCbits.RC7
#define BUZZER PORTCbits.RC0
#define SET PORTCbits.RC1
#define PLUS PORTCbits.RC2
#define MOINS PORTCbits.RC3
#define SNOOZE PORTCbits.RC4
#define POS3_1 PORTCbits.RC6
#define POS3_3 PORTCbits.RC5

typedef enum cligno
{
	AUCUN = 0,
	GAUCHE = 1,
	DROITE = 2,
	TOUS = 3
} Clignotement;

typedef enum aff
{
	DATE,
	HEURE,
	ALARME
} EtatAffichage;

char etatModification = 0; // 1 : heure, 2 : minute, 3 : jour, 4 : mois, 5 : heureAlarme, 6 : minuteAlarme
char affDate = 0;
char heure = 0, minute = 0, seconde = 0, mois = 1, jour = 1, heureAlarme = 0, minuteAlarme = 0;
char setPreced = 0, plusPreced = 0, moinsPreced = 0, pos3Preced = 0, snoozePreced = 0;
char heureSnooze = 0, minuteSnooze = 0, snoozeOn = 0;
char alarmeEnMarche = 0;

void wait(long microsecondes)
{
	long i = 0;

	for (; i < microsecondes; i++);
}
//Fonction à écrire
// arrête le programme pendant précisément la periode donnée en paramètre
// période en millisecondes comprise entre 0 et 32767

EtatAffichage etatAffichage()
{
	if (POS3_1 && !affDate)
		return HEURE;
	else if (POS3_1)
		return DATE;
	else if (POS3_3 && etatModification <= 2)
		return HEURE;
	else if (POS3_3)
		return DATE;
	else
		return ALARME;
}

Clignotement etatClignotement()
{
	if (etatModification == 0)
		return AUCUN;
	else if (etatModification%2 == 1)
		return GAUCHE;
	else
		return DROITE;
}

void faireSet()
{
	if (POS3_3)
		etatModification = etatModification%4+1;
	else if (POS3_1)
		affDate = !affDate;
	else
		etatModification = etatModification%2+5;
}

void fairePlus()
{
	char maxJour;
	if (POS3_3)
	{
		switch (etatModification)
		{
			case 1:
				heure = heure++%24;
				break;

			case 2:
				minute = minute++%60;
				break;

			case 3:
				jour++;
				if ((mois <= 7 && mois%2 == 1) || (mois > 7 && mois%2 == 0))
					maxJour = 31;
				else if (mois == 2)
					maxJour = 28;
				else
					maxJour = 30;

				if (jour > maxJour)
				{
					jour -= maxJour;
				}

				break;

			case 4:
				mois = mois%12+1;
				break;
		}
	}
	else if (!POS3_1)
	{
		switch (etatModification)
		{
			case 5:
				heureAlarme = heureAlarme++%24;
				break;

			case 6:
				minuteAlarme = minuteAlarme++%60;
		}
	}
}

void faireMoins()
{
	char maxJour;
	if (POS3_3)
	{
		switch (etatModification)
		{
			case 1:
				heure = heure == 0 ? 23 : heure--;
				break;

			case 2:
				minute = minute == 0 ? 59 : minute--;
				break;

			case 3:
				if ((mois <= 7 && mois % 2 == 1) || (mois > 7 && mois % 2 == 0))
					maxJour = 31;
				else if (mois == 2)
					maxJour = 28;
				else
					maxJour = 30;

				jour = jour == 1 ? maxJour : jour--;

				break;

			case 4:
				mois = mois == 1 ? 12 : mois--;
				break;
		}
	}
	else if (!POS3_1)
	{
		switch (etatModification)
		{
			case 5:
				heureAlarme = heureAlarme == 0 ? 23 : heureAlarme--;
				break;

			case 6:
				minuteAlarme = minuteAlarme == 0 ? 59 : minuteAlarme--;
		}
	}
}

void faireSnooze()
{
	if (INTERR_ALARME && alarmeEnMarche)
	{
		snoozeOn = 1;
		heureSnooze = heure;
		minuteSnooze = minute;
		BUZZER = 0;
	}
}

char decimalToBCD(char chiffre)
// convertit un chiffre en valeur d'affichage par le 7 segments
// chiffre compris entre 0 et 9 (/!\ valeur 0 et 9, et non le caractère '0' et '9')
// sorties sur 8 bits. Codage des bits, de poids fort vers poids faible -> Dp, g, f, e, d, c, b, a
// segment éteint = 0
{
	char valeurs[10] = {0b00111111,
						0b00000110,
						0b01011011,
						0b01001111,
						0b01100110,
						0b01101101,
						0b01111101,
						0b00000111,
						0b01111111,
						0b01101111}; // Valeurs BCD de chaque chiffre
	
	if (chiffre < 0 || chiffre > 9)
		return 0b01000000; // Retourne "-" en BCD si valeur de chiffre non respectée
	
	return valeurs[chiffre];
}

void tabToBCD(char chiffre[])
{
	char i;
	for (i = 0; i < 4; i++)
	{
		chiffre[i] = decimalToBCD(chiffre[i]);
	}
}

void decomposer(int nombre, char destination[])
//Récupère nombre compris entre 0 et 9999
//Isole les 4 digits (décimaux) du nombre dans tableau de 4 variables (stockés de poids fort vers poids faible -> 2684 : {2, 6, 8, 4})
{
	while (nombre >= 10000) //Garantit que nombre inferieur à 10000
		nombre -= 10000;
	
	while (nombre >= 1000) //Isole digit des milliers
	{
		nombre -= 1000;
		destination[0]++;
	}
	
	while (nombre >= 100) //Isole digit des centaines
	{
		nombre -= 100;
		destination[1]++;
	}
	
	while (nombre >= 10) //Isole digit des dizaines
	{
		nombre -= 10;
		destination[2]++;
	}
	
	while (nombre >= 1) //Isole digit des unités
	{
		nombre -= 1;
		destination[3]++;
	}
}

void afficher(char chiffres[], Clignotement cligno, char afficherCligno, char led)
//Affiche un set de 4 chiffres (décimaux) sur les 4 afficheurs, en faisant clignoter ou non chaque duo suivant la valeur de cligno et du "booléen" afficherCligno
//Tableau de 4 codes d'afficheurs 7 segments (tels que renvoyés par la fonction decimalToBCD
//Fonction devant attendre temps donné pour chaque afficheur pour assurer la bonne vision des afficheurs
{
	char i = 0b0;

	if (led)
		chiffres[3] += 0b10000000;

	for (i; i < 4; i++)
	{
		MULTI_FORT = i>1; //Broche poids fort du multiplexeur
		MULTI_FAIBLE = i%2; //Broche poids faible du multiplexeur
		if (afficherCligno || ((i < 2 && cligno%2 == 0) || (i > 2 && cligno < 2))) //On vérifie que l'on doit bel et bien afficher ce nombre (on doit afficher les nombres clignotant, ou bien le nombre n'est pas concerné par le clignotant
			AFFICHEUR = chiffres[i]; //On donne les bits d'affichage de l'afficheur
		else
			AFFICHEUR = 0;
		
		wait(PERIODE_AFF); //On attend pour que l'afficheur reste allumé un certain temps
	}
	
	AFFICHEUR = 0;
}

void setLedsEcran(int allumer, int basEtHaut)
{
	LED_HAUT = 0b0;
	LED_BAS = 0b0;
	
	if (allumer)
	{
		LED_BAS = 0b1;
		
		if (basEtHaut)
			LED_HAUT = 0b1;
	}
}

int main(int argc, char *argv[])
//Fonction principale du programme. Cette fonction n'est pas complète et sera mise à jour régulièrement
//Certaines variables de cette fonction pourront devenir des variables globales, nottemment pour pouvoir être accessibles par les interruptions
//1e MAJ : 01/02/16
{
	char affichage[4];
	
	while (1)
	{
		if (SET)
		{
			if (setPreced == 0 || setPreced == 2)
			{
				faireSet();
				setPreced = 1;
			}
		}
		else
			setPreced = 0;

		if (PLUS)
		{
			if (plusPreced == 0 || plusPreced == 2)
			{
				fairePlus();
				plusPreced = 1;
			}
		}
		else
			plusPreced = 0;

		if (MOINS)
		{
			if (moinsPreced == 0 || moinsPreced == 2)
			{
				faireMoins();
				moinsPreced = 1;
			}
		}
		else
			moinsPreced = 0;

		if (SNOOZE)
		{
			if (snoozePreced == 0)
				faireSnooze();
		}
		else
			snoozePreced = 0;

		if (INTERR_ALARME &&
				((!snoozeOn && heureAlarme == heure && minuteAlarme == minute) ||
				(snoozeOn && heure == heureSnooze+((minuteSnooze + 5)/60) && minute == (minuteSnooze+5)/60)))
		{
			alarmeEnMarche = 1;
			BUZZER = 1;
		}
		
		if (!INTERR_ALARME ||
			alarmeEnMarche &&
				((!snoozeOn && heure == heureAlarme+((minuteAlarme + 5)/60) && minute == (minuteAlarme+5)%60) ||
				(snoozeOn && heure == heureSnooze+((minuteSnooze+10)/60) && minute == (minuteSnooze+10)/60)))
		{
			alarmeEnMarche = 0;
			BUZZER = 0;
			snoozeOn = 0;
		}
		
		if (etatAffichage() == DATE)
		{
			setLedsEcran(1, 0);

			decomposer(jour*100 + mois, affichage);
			tabToBCD(affichage);

			afficher(affichage, etatClignotement(), !seconde, INTERR_ALARME);
		}
		else
		{
			setLedsEcran(seconde, 1);
			if (etatAffichage() == HEURE)
				decomposer(heure*100 + minute, affichage);
			else
				decomposer(heureAlarme*100 + minuteAlarme, affichage);
			tabToBCD(affichage);

			afficher(affichage, etatClignotement(), !seconde, INTERR_ALARME);
		}
	}
	
	return 0;
}
