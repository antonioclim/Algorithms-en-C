# Temă Săptămâna 1: Pointeri la Funcții și Callbacks

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 2
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flag-urile `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Sistem de Filtrare Generică (50 puncte)

### Descriere

Implementați un sistem generic de filtrare a datelor folosind funcții callback. Sistemul trebuie să poată filtra:
- Array-uri de numere întregi
- Array-uri de structuri (produse dintr-un magazin)

### Cerințe

1. **Definiți structura `Product`** (10p):
   ```c
   typedef struct {
       int id;
       char name[50];
       float price;
       int quantity;
   } Product;
   ```

2. **Implementați funcția generică de filtrare** (15p):
   ```c
   int filter_array(void *src, void *dest, int n, size_t elem_size,
                    int (*predicate)(const void *));
   ```
   - `src`: array-ul sursă
   - `dest`: array-ul destinație (pre-alocat)
   - `n`: numărul de elemente
   - `elem_size`: dimensiunea unui element
   - `predicate`: funcția de filtrare (returnează 1 pentru elementele păstrate)
   - **Return:** numărul de elemente filtrate

3. **Implementați predicate pentru numere** (10p):
   - `is_positive(const void *elem)` - returnează 1 dacă numărul > 0
   - `is_even(const void *elem)` - returnează 1 dacă numărul este par
   - `is_greater_than_10(const void *elem)` - returnează 1 dacă numărul > 10

4. **Implementați predicate pentru produse** (10p):
   - `is_in_stock(const void *elem)` - returnează 1 dacă quantity > 0
   - `is_expensive(const void *elem)` - returnează 1 dacă price > 100.0
   - `is_low_stock(const void *elem)` - returnează 1 dacă quantity < 5

5. **Funcția main cu demonstrație** (5p):
   - Demonstrați filtrarea pe un array de 10 numere
   - Demonstrați filtrarea pe un array de 5 produse

### Exemplu de Utilizare

```c
int numbers[] = {-5, 12, 3, -8, 7, 0, 15, -2, 9, 4};
int positive[10];
int count = filter_array(numbers, positive, 10, sizeof(int), is_positive);
// count = 6 (numerele: 12, 3, 7, 15, 9, 4)
```

### Fișier: `tema1_filter.c`

---

## 📝 Tema 2: Parser de Expresii Matematice (50 puncte)

### Descriere

Implementați un evaluator de expresii matematice simple care folosește un dispatch table pentru operații.

### Cerințe

1. **Structura expresiei** (10p):
   ```c
   typedef struct {
       int operand1;
       char operator;
       int operand2;
   } Expression;
   ```

2. **Dispatch table cu operații** (15p):
   - Implementați toate operațiile: `+`, `-`, `*`, `/`, `%`, `^` (putere)
   - Tratați cazurile speciale (împărțire la 0, overflow la putere)

3. **Parser de expresii din șir** (15p):
   ```c
   int parse_expression(const char *str, Expression *expr);
   ```
   - Parsează un șir de forma "operand1 operator operand2"
   - Returnează 1 la succes, 0 la eroare
   - Tratează spații multiple și input invalid

4. **Evaluator de expresii** (5p):
   ```c
   int evaluate(const Expression *expr, int *result);
   ```
   - Returnează 1 la succes, 0 la eroare (operator necunoscut, div by 0)

5. **Program interactiv** (5p):
   - Citește expresii de la tastatură
   - Afișează rezultatul sau mesaj de eroare
   - Continuă până la "exit" sau EOF

### Exemplu de Sesiune

```
Calculator de Expresii
> 5 + 3
5 + 3 = 8
> 2 ^ 10
2 ^ 10 = 1024
> 10 / 0
Eroare: Împărțire la zero
> exit
La revedere!
```

### Fișier: `tema2_parser.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizare corectă a pointerilor la funcții | 25 |
| Tratarea cazurilor speciale | 15 |
| Calitatea codului (stil, comentarii) | 10 |
| Compilare fără warning-uri | 10 |

### Penalizări

- -10p: Compilare cu warning-uri
- -20p: Memory leaks (verificat cu Valgrind)
- -30p: Crash la input valid
- -50p: Plagiat (se aplică la ambele părți)

---

## 📤 Modalitate de Predare

1. Creați fișierele `tema1_filter.c` și `tema2_parser.c`
2. Verificați compilarea: `gcc -Wall -Wextra -std=c11 -o tema1 tema1_filter.c`
3. Verificați memoria: `valgrind ./tema1`
4. Încărcați pe platforma de cursuri

---

## 💡 Sugestii

1. **Începeți cu structurile** - definiți-le corect înainte de funcții
2. **Testați incremental** - verificați fiecare funcție separat
3. **Folosiți typedef** - pentru pointeri la funcții
4. **Comentați codul** - explicați logica complexă
5. **Tratați toate cazurile** - input invalid, overflow, etc.

---

## 🔗 Resurse Utile

- `man qsort` - documentația funcției qsort
- K&R Capitol 5.11 - Pointeri la funcții
- Slide-urile din `slides/presentation-week01.html`

---

*Succes! 🎯*
