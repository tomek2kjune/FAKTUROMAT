#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

//struct for position
typedef struct
{
	char item[33];
	float qty;
	float unitCost;
	float totalCost;
}pos;

//struct for client
typedef struct
{
	char name[83];
	char street[33];
	char city[33];
	char zip[9];
	char TIN[13];
}client;

//struct for invoice
typedef struct
{
	char number[13];
	char date[13];
	client clientData;
	pos posData;
}invoice;

//element of list
typedef struct
{
	struct element* previous;
	invoice data;
	struct element* next;
}element;

//check if list empty
int isEmpty(element* first)
{
	if (first == NULL)
	{
		printf("\n!> Baza faktur jest pusta <!");
		return 1;
	}
}

//get invoice number
char* getNumber(int number)
{
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	char date[12];

	sprintf(date, "%d/%d/%d", number, tm->tm_mon + 1, tm->tm_year + 1900);

	return date;
}

//get current date
char* getDate()
{
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	char date[12];

	sprintf(date, "%d/%d/%d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);

	return date;
}

//replace \n with \0 in string and clear stdin
void rAcBuffer(char* buffer)
{
	char* ptr;

	if ((ptr = strchr(buffer, '\n')) != NULL)
	{
		*ptr = '\0';
	}
	else {
		while (getchar() != '\n');
	}
}

//create new invoice
element* createInvoice(int number, element* newElement)
{
	strcpy(newElement->data.number, getNumber(number));
	strcpy(newElement->data.date, getDate());

	printf(">>Dane klienta<<\n");
	printf("Nazwa kontrahenta [max 80 znaków]: ");
	getchar();
	fgets(newElement->data.clientData.name, 80, stdin);
	rAcBuffer(newElement->data.clientData.name);
	printf("Ulica i numer domu [max 30 znaków]: ");
	fgets(newElement->data.clientData.street, 32, stdin);
	rAcBuffer(newElement->data.clientData.street);
	printf("Miasto [max 30 znaków]: ");
	fgets(newElement->data.clientData.city, 32, stdin);
	rAcBuffer(newElement->data.clientData.city);
	printf("Kod pocztowy [max 6 znaków]: ");
	fgets(newElement->data.clientData.zip, 8, stdin);
	rAcBuffer(newElement->data.clientData.zip);
	printf("NIP [max 10 znaków]: ");
	fgets(newElement->data.clientData.TIN, 12, stdin);
	rAcBuffer(newElement->data.clientData.TIN);

	printf("\n>>Pozycja<<\n");
	printf("Nazwa towaru [max 30 znaków]: ");
	fgets(newElement->data.posData.item, 30, stdin);
	rAcBuffer(newElement->data.posData.item);
	while (1)
	{
		printf("Ilość [max 1000000,00]: ");
		scanf("%f", &newElement->data.posData.qty);
		if (newElement->data.posData.qty < 1000000)
		{
			break;
		}
		else
		{
			printf("\n!> Wprowadzona ilość przekracza limit <!\n\n");
		}
	}
	printf("Cena jednostkowa: ");
	scanf("%f", &newElement->data.posData.unitCost);
	newElement->data.posData.totalCost = newElement->data.posData.qty * newElement->data.posData.unitCost;
}

//import invoice from file
element* importInvoice(int number, element* newElement)
{
	char name[16];
	char fname[25];
	char string[110];
	char* sub;
	
	while (1)
	{
		printf("Podaj nazwę pliku z fakturą do wczytania (bez rozszerzenia, max 15 znaków): ");
		scanf("%s", name);
		sprintf(fname, "%s.txt", name);
		FILE* file;
		if (file = fopen(fname, "r"))
		{
			fopen(fname, "r");
			fgets(string, 100, file); //thrash Faktura numer: x/x/x
			strcpy(newElement->data.number, getNumber(number)); //get number
			fscanf(file, "%s", string); //thrash Data
			fscanf(file, "%s", string); //thrash wystawienia:
			fscanf(file, "%s", newElement->data.date); //get date
			for (int i = 0; i < 9; i++)
			{
			fgets(string, 100, file); //thrash 9 lines
			}		
			fgets(string, 100, file); //get name
			rAcBuffer(string);
			strcpy(newElement->data.clientData.name, string);
			fgets(string, 100, file); //get street
			rAcBuffer(string);
			strcpy(newElement->data.clientData.street, string);
			fgets(string, 100, file); //get city, zip line
			sub = strtok(string, ","); //get city
			strcpy(newElement->data.clientData.city, sub);
			sub = strtok(NULL, "\n"); //get zip
			strcpy(newElement->data.clientData.zip, sub);
			fgets(string, 100, file); //get TIN line
			sub = strtok(string, ":"); //thrash NIP:
			sub = strtok(NULL, "\n"); //get tin
			strcpy(newElement->data.clientData.TIN, sub);
			for (int i = 0; i < 3; i++)
			{
				fgets(string, 100, file); //thrash 3 lines
			}
			fgets(string, 100, file); //get position line
			sub = strtok(string, "|"); //get item
			sub[strlen(sub) - 1] = '\0';
			strcpy(newElement->data.posData.item, sub);
			sub = strtok(NULL, "|"); //get qty
			newElement->data.posData.qty = atof(sub);
			sub = strtok(NULL, "|"); //get unitCost
			newElement->data.posData.unitCost = atof(sub);
			newElement->data.posData.totalCost = newElement->data.posData.qty * newElement->data.posData.unitCost;
			fclose(file);
			break;
		}
		else
		{
			printf("\n!> Plik nie istnieje, lub posiada zbyt długą nazwę <!");
			Sleep(1000);
			system("CLS");
		}
	}

	return newElement;
}

//add invoice to base
element* addInvoice(int number, element* first)
{
	element* newElement = malloc(sizeof(element));
	char input;

	while (1)
	{
		system("CLS");
		printf("OPCJE:\n");
		printf("[t] stwórz fakturę\n");
		printf("[i] importuj fakturę\n");
		scanf(" %c", &input);
		system("CLS");
		if (input == 't' || input == 'T')
		{
			newElement = createInvoice(number, newElement);
			break;
		}
		else if (input == 'i' || input == 'I')
		{
			newElement = importInvoice(number, newElement);
			break;
		}
		else
		{
			printf("\n!> Brak takiej opcji <!\n\n");
			Sleep(1000);
		}
	}

	printf("\n> Dodano fakturę <");

	newElement->previous = NULL;
	newElement->next = NULL;
	if (first == NULL) {
		first = newElement;
	}
	else
	{
		first->previous = newElement;
		newElement->next = first;
		first = newElement;
	}

	return first;
}

//print list in console
int printList(element* first)
{ 
	int index = 0;

	element* current = first;

	printf("______________________________________________________________________\n");
	printf("POZ. NUMER FAKTURY | DATA WYSTAWIENIA |                        NABYWCA\n");
	printf("----------------------------------------------------------------------\n");

	while (current != NULL)
	{
		index++;
		printf("%3d. %13s | %16s | %30s\n", index, current->data.number, current->data.date, current->data.clientData.name);
		current = current->next;
	}

	return index;
}

//return requested element
element* returnElement(int pos, element* first)
{
	element* req = first;

	for (int i = 1; i < pos; i++)
	{
		req = req->next;
	}

	return req;
}

//print exact element from list
void printElement(int index, element* first)
{
	int pos;

	element* req;

	while (1)
	{
		printf("\nWprowadź pozycję do wyświetlenia: ");
		scanf("%d", &pos);
		if (pos <= index && pos > 0)
		{
			req = returnElement(pos, first);
			break;
		}
		else
		{
			printf("\n!> Nie ma takiej pozycji <!");
			Sleep(1000);
		}
	}

	system("CLS");
	printf("   Faktura numer: %s\n", req->data.number);
	printf("Data wystawienia: %s\n", req->data.date);
	printf("\n");
	printf(">SPRZEDAWCA<\n");
	printf("mojaspółka.pl sp.k.\n");
	printf("Karakanowa 12\n");
	printf("Bikini Dolne,01-101\n");
	printf("NIP:7968228697\n");
	printf("\n");
	printf(">NABYWCA<\n");
	printf("%s\n", req->data.clientData.name);
	printf("%s\n", req->data.clientData.street);
	printf("%s,%s\n", req->data.clientData.city, req->data.clientData.zip);
	printf("NIP:%s\n", req->data.clientData.TIN);
	printf("\n");
	printf("POZYCJA                       |      ILOŚĆ | CENA JEDNOSTKOWA | RAZEM\n");
	printf("%29s | %10.2f | %16.2f | %.2f", req->data.posData.item, req->data.posData.qty, req->data.posData.unitCost, req->data.posData.totalCost);
}

//replace slash with floor
void replaceSwF(char* buffer)
{
	char* ptr;

	while ((ptr = strchr(buffer, '/')) != NULL)
	{
		*ptr = '_';
	}
}

//save exact element to file
void saveElement(int index, element* first)
{
	int pos;
	char fname[20];
	char* ptr;

	element* req;

	while (1)
	{
		printf("\nWprowadź pozycję do zapisania: ");
		scanf("%d", &pos);
		if (pos <= index && pos > 0)
		{
			req = returnElement(pos, first);
			break;
		}
		else
		{
			printf("\n!> Nie ma takiej pozycji <!");
			Sleep(1000);
		}
	}

	sprintf(fname, "%s.txt", req->data.number);
	
	replaceSwF(fname);

	FILE* file;
	file = fopen(fname, "w+");
	fprintf_s(file, "   Faktura numer: %s\n", req->data.number);
	fprintf_s(file, "Data wystawienia: %s\n", req->data.date);
	fprintf_s(file, "\n");
	fprintf_s(file, ">SPRZEDAWCA<\n");
	fprintf_s(file, "mojaspółka.pl sp.k.\n");
	fprintf_s(file, "Karakanowa 12\n");
	fprintf_s(file, "Bikini Dolne,01-101\n");
	fprintf_s(file, "NIP:7968228697\n");
	fprintf_s(file, "\n");
	fprintf_s(file, ">NABYWCA<\n");
	fprintf_s(file, "%s\n", req->data.clientData.name);
	fprintf_s(file, "%s\n", req->data.clientData.street);
	fprintf_s(file, "%s,%s\n", req->data.clientData.city, req->data.clientData.zip);
	fprintf_s(file, "NIP:%s\n", req->data.clientData.TIN);
	fprintf_s(file, "\n");
	fprintf_s(file, "POZYCJA                       |      ILOŚĆ | CENA JEDNOSTKOWA | RAZEM\n");
	fprintf_s(file, "%29s | %10.2f | %16.2f | %.2f\n", req->data.posData.item, req->data.posData.qty, req->data.posData.unitCost, req->data.posData.totalCost);
	fclose(file);

	printf("\n> Fakturê zapisano <");
}

//delete first element of list
element* delFirst(element* first)
{
	element* del = first;
	first = first->next;
	if (first != NULL)
	{
		first->previous = NULL;
	}

	free(del);

	return first;
}

//delete last element of list
void delLast(element* first)
{
	element* last = first;

	while (last->next != NULL)
	{
		last = last->next;
	}

	element* del = last;
	last = last->previous;

	if (last != NULL)
	{
		last->next = NULL;
	}

	free(del);
}

//delete n element from list
element* delN(element* first, int index)
{
	element* current;

	int pos;

	current = first;

	element* prev;
	element* nxt;

	while (1)
	{
		printf("\nWprowadź pozycję do usunięcia: ");
		scanf("%d", &pos);

		if (pos == 1)
		{
			first = delFirst(first);
			break;
		}
		else if (pos == index)
		{
			delLast(first);
			break;
		}
		else if (pos > 1 && pos < index)
		{
			for (int i = 1; i < pos; i++)
			{
				current = current->next;
			}
			prev = current->previous;
			nxt = current->next;
			prev->next = current->next;
			nxt->previous = current->previous;

			free(current);

			break;
		}
		else
		{
			printf("\n!> Nie ma takiej pozycji <!");
			Sleep(1000);
		}
	}
	printf("\n> Usunięto pozycję numer: %d <", pos);

	return first;
}

//options for exact element
element* quickMenu(element* first, int index)
{
	if (isEmpty(first) == 1)
	{
		return first;
	}

	char input;

	while (1)
	{
		printf("\nOPCJE:\n");
		printf("[w] wyświetl fakturę\n");
		printf("[z] zapisz fakturę do pliku\n");
		printf("[u] usuń fakturę\n");
		printf("[e] wyjdź\n");
		scanf(" %c", &input);
		if (input == 'w' || input == 'W')
		{
			printElement(index, first);
			break;
		}
		else if (input == 'z' || input == 'Z')
		{
			saveElement(index, first);
			break;
		}
		else if (input == 'u' || input == 'U')
		{
			first = delN(first, index);
			break;
		}
		else if (input == 'e' || input == 'E')
		{
			break;
		}
		else
		{
			printf("\n!> Brak takiej opcji <!\n");
			Sleep(1000);
			printf("\r \r");
		}
	}

	return first;
}

//find string by search in string
char* stristr(char* strToS, char* strToF)
{
	char strToS_Up[80];
	char strToF_Up[80];

	strcpy(strToS_Up, strToS);
	strcpy(strToF_Up, strToF);

	_strupr(strToS_Up);
	_strupr(strToF_Up);

	return strstr(strToS_Up, strToF_Up);
}

//print exact position from list in console
void printPositionC(int index, element* req)
{
	printf("%3d. %13s | %16s | %30s\n", index, req->data.number, req->data.date, req->data.clientData.name);
}

//find exact elements
void find(element* first)
{
	if (isEmpty(first) == 1)
	{
		return;
	}

	element* current = first;
	char prefix[80];
	int everPrinted = FALSE;
	int index = 0;

	printf("Wprowadź fragment nazwy kontrahenta: ");
	scanf("%s", prefix);

	printf("______________________________________________________________________\n");
	printf("POZ. NUMER FAKTURY | DATA WYSTAWIENIA |                        NABYWCA\n");
	printf("----------------------------------------------------------------------\n");

	while (current != NULL)
	{
		if (stristr(current->data.clientData.name, prefix))
		{
			index++;
			printPositionC(index, current);
			everPrinted = TRUE;
		}
		current = current->next;
	}

	if (everPrinted == FALSE)
	{
		printf("\n!> Nie odnaleziono pozycji <!");
	}
}

//print exact position from list in file
void printPositionF(FILE* const file, int index, element* req)
{
	fprintf_s(file ,"%3d. %13s | %16s | %30s | %.2f\n", index, req->data.number, req->data.date, req->data.clientData.name, req->data.posData.totalCost);
}

//save monthly statement to file
void saveMS(element* first)
{
	if (isEmpty(first) == 1)
	{
		return;
	}

	element* current = first;
	char prefix[4];
	int everExist = FALSE;
	int index = 0;
	char fname[30];
	float sum = 0;
	char sPrefix[6];

	system("CLS");
	printf("Wprowadź numer miesiąca, dla którego przygotować rozliczenie miesięczne: ");
	scanf("%s", prefix);

	sprintf(sPrefix, "/%s/", prefix);

	while (current != NULL)
	{
		if (strstr(current->data.date, sPrefix))
		{
			everExist = TRUE;
		}
		current = current->next;
	}

	if (everExist == FALSE)
	{
		printf("\n!> Brak faktur dla tego miesiąca <!");
		return;
	}

	current = first;

	sprintf(fname, "ROZLICZENIE_MIESIĄC-%s.txt", prefix);

	FILE* file;
	file = fopen(fname, "w+");
	srand(time(NULL));
	fprintf_s(file, "\nUNIKATOWY HASH DOKUMENTU: #%d\n\n", rand() % 100 + 1800);
	fprintf_s(file, "ROZLICZENIE ZA %s MIESIĄC\n", prefix);
	fprintf_s(file, "DLA FIRMY:\n\n");
	fprintf_s(file, "mojaspółka.pl sp.k.\n");
	fprintf_s(file, "Karakanowa 12\n");
	fprintf_s(file, "Bikini Dolne,01-101\n");
	fprintf_s(file, "NIP:7968228697\n\n");
	fprintf_s(file, "________________________________________________________________________________________\n");
	fprintf_s(file, "POZ. NUMER FAKTURY | DATA WYSTAWIENIA |                        NABYWCA | WART. SPRZEDA¯Y\n");
	fprintf_s(file, "----------------------------------------------------------------------------------------\n");

	while (current != NULL)
	{
		if (strstr(current->data.date, sPrefix))
		{
			index++;
			printPositionF(file, index, current);
			sum = sum + current->data.posData.totalCost;
		}
		current = current->next;
	}

	fprintf_s(file, "\n\nROCZNA WARTOŚĆ SPRZEDAŻY WYNIOSŁA:                                       %.2f", sum);
	fclose(file);

	printf("\n> Wykonano rozliczenie <");
}

//clear list
void clearList(element* first)
{
	element* current = first;
	element* next;
	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}
}

//entry function
void main()
{
	element* first = NULL;
	int number = 1;
	int index = 0;
	int x;

	setlocale(LC_ALL, "Polish");

	while (1)
	{
		system("CLS");
		printf("\x1B[35m+++++++++++++++++++++++++++++++++++++++++++\033[0m\n");
		printf("\x1B[35m+\033[0m \x1B[32mFAKTUROMAT mojaspółka.pl sp.k. ver. 1.0\033[0m \x1B[35m+\033[0m\n");
		printf("\x1B[35m+++++++++++++++++++++++++++++++++++++++++++\033[0m\n\n");
		printf("MENU:\n");
		printf("[1] Dodaj fakturę\n"); //create or import invoice from file and add it to base
		printf("[2] Baza faktur\n"); //print positions list and print exact invoice or save it to file or delete it from base
		printf("[3] Wyszukiwanie faktur\n"); //find positions assigned to exact client and print them
		printf("[4] Rozliczenie miesięczne\n"); //print monthly statement with random hash to file
		printf("[0] Wyjście\n"); //exiting program
		scanf("%d", &x);
		system("CLS");

		switch (x)
		{
			case 1:
				first = addInvoice(number, first);
				number++;
				break;
			case 2:
				index = printList(first);
				first = quickMenu(first, index);
				break;
			case 3:
				find(first);
				break;
			case 4:
				saveMS(first);
				break;
			case 0:
				clearList(first);
				exit(EXIT_SUCCESS);
				break;
			default:
				system("CLS");
				printf("\n!> Brak takiej opcji <!");
				break;
		}
		printf("\n\nWciśnij [Enter], aby kontynuować\n");
		getchar();
		while (getchar() != '\n');
	}
}