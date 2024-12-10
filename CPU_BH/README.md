# Symulacja N-ciał na CPU (Barnes-Hut method)

## Opis
Projekt implementuje algorytm Barnes-Hut do symulacji dynamiki N ciał na CPU. Barnes-Hut to efektywna metoda oparta na hierarchicznej dekompozycji przestrzeni, która pozwala na przybliżone obliczenie sił grawitacyjnych między ciałami w czasie logarytmicznym względem liczby obiektów. Symulacja modeluje ruch i interakcje ciał pod wpływem grawitacji, uwzględniając warunki takie jak stabilność numeryczna i efektywność obliczeniową.
Implementacja została zoptymalizowana do działania na procesorach (CPU) z użyciem **OpenMP** do równoległego przetwarzania. OpenMP to biblioteka umożliwiająca równoległe wykonywanie kodu na wielu rdzeniach procesora. Jest łatwa w implementacji i integracji, co czyni ją idealnym rozwiązaniem dla obliczeń z dużą liczbą iteracji, takich jak symulacje N-ciał.

---

## Spis treści
1. [Opis](#opis)
2. [Struktura projektu](#struktura-projektu)
3. [Instalacja](#instalacja)
4. [Użycie](#użycie)
5. [Szczegóły implementacji](#szczegóły-implementacji)
6. [Wydajność i optymalizacje](#wydajność-i-optymalizacje)
7. [Wnioski](#wnioski)
8. [Licencja](#licencja)

---

## Struktura projektu
- `src/`
  - **`main.cpp`**: Główna logika sterująca symulacją.
  - **`BHTreeNode.cpp`**: Implementacja drzewa Barnes-Hut.
  - **`Body.cpp`**: Definicja ciał w symulacji.
  - **`Octant.cpp`**: Zarządzanie oktantami w przestrzeni.
  - **`Simulation.cpp`**: Funkcje symulacji, w tym integrator ruchu i budowa drzewa.
  - **`BHTreeNode.h`**, **`Body.h`**, **`Octant.h`**, **`Simulation.h`**: Nagłówki zawierające definicje klas i funkcji.
- `tests/`
  - **`BHTreeNodeTest.cpp`**, **`BodyTest.cpp`**, **`OctantTest.cpp`**, **`SimulationTest.cpp`**: Testy weryfikujące poprawność implementacji.
- `CMakeLists.txt`: Konfiguracja budowania projektu za pomocą **CMake**, w tym konfiguracja zależności jak OpenMP.

---

## Instalacja
### Wymagania
- Kompilator zgodny z C++17 (np. GCC, Clang, MSVC).
- **CMake** (wersja 3.8 lub wyższa).
- Obsługa OpenMP.

### Instrukcja budowy
1. Sklonuj repozytorium:
```bash
git clone <adres_repozytorium>
cd <nazwa_repozytorium>
```

2. Zbuduj projekt:
```bash
mkdir build
cd build
cmake ..
make
```

3. (Opcjonalnie) Uruchom testy:
```bash
ctest
```

---

## Użycie
Symulacja jest inicjowana z predefiniowanymi ciałami w pliku `main.cpp`. Użytkownik może:
- Zmieniać liczbę ciał i ich początkowe parametry.
- Modyfikować liczbę kroków symulacji (zmienna `steps`).
- Analizować dane wyjściowe, takie jak pozycje i prędkości w konsoli.

---

## Szczegóły implementacji
**1. Algorytm Barnes-Hut**  
Algorytm Barnes-Hut opiera się na hierarchicznej dekompozycji przestrzeni, która pozwala efektywnie obliczać siły grawitacyjne między dużą liczbą ciał. Implementacja obejmuje:

- **Podział przestrzeni na oktanty**:  
  Przestrzeń jest dzielona na osiem równych regionów (oktantów) w formie drzewa oktantowego. Każdy węzeł drzewa odpowiada określonemu obszarowi przestrzeni. Węzły mogą być:
  - **Liśćmi**: Reprezentują pojedyncze ciało w przestrzeni.
  - **Węzłami wewnętrznymi**: Reprezentują grupę ciał, dla których obliczono skumulowaną masę i położenie środka masy.  

- **Przybliżenie sił grawitacyjnych**:  
  - Dla ciał znajdujących się w dużej odległości od badanego ciała, węzły wewnętrzne traktuje się jako pojedynczy punkt masowy.  
  - Wykorzystuje się parametr **𝜃 (THETA)**, który kontroluje dokładność obliczeń. Jeśli stosunek rozmiaru regionu do odległości od badanego ciała jest mniejszy niż 𝜃, cała masa regionu jest traktowana jako skoncentrowana w jego środku masy.  
  - Dla regionów bliższych obliczenia sił są wykonywane rekurencyjnie na podregionach (dzieciach węzła).  

- **Hierarchiczna budowa drzewa**:  
  Drzewo oktantowe jest budowane na podstawie wszystkich ciał w przestrzeni, zaczynając od korzenia reprezentującego cały obszar symulacji. Każde ciało jest przypisywane do odpowiedniego podregionu, aż do osiągnięcia poziomu liścia.

---

**2. Integrator Leapfrog**  
Integrator Leapfrog jest stabilną metodą numeryczną stosowaną do rozwiązywania równań ruchu. Jego kluczowe cechy w implementacji:

- **Aktualizacja pozycji i prędkości w dwóch krokach**:  
  - Najpierw prędkości są aktualizowane o połowę kroku czasowego na podstawie sił działających na ciało.  
  - Pozycje ciał są aktualizowane na podstawie prędkości.  
  - Na końcu prędkości są aktualizowane o drugą połowę kroku czasowego, uwzględniając zmienione siły.  

- **Zalety Leapfrog**:  
  - Zachowuje stabilność nawet przy dużych krokach czasowych.  
  - Jest symplektyczny, co oznacza, że lepiej zachowuje energię całkowitą układu w długim okresie czasu w porównaniu do prostszych metod, takich jak Euler.

---

**3. Efektywność obliczeniowa**

- **Złożoność obliczeniowa**:  
  Algorytm Barnes-Hut redukuje złożoność obliczeniową z O(N^2) (dla par ciał w metodzie naiwnej) do O(N logN).  
  - Dzieje się tak dzięki hierarchicznemu podziałowi przestrzeni, który pozwala grupować odległe ciała i wykonywać dla nich przybliżone obliczenia.  
  - Rekurencyjne przetwarzanie węzłów pozwala uniknąć konieczności szczegółowego analizowania sił między ciałami w różnych oktantach.

- **Równoległość z OpenMP**:  
  - Obliczenia sił dla każdego ciała są niezależne, co pozwala na łatwą równoległość.  
  - Dzięki dyrektywie `#pragma omp parallel for`, obliczenia dla każdego ciała są rozdzielane między rdzenie procesora.  
  - Wydajność zwiększa się znacząco dla dużych zestawów danych, szczególnie na procesorach wielordzeniowych.

- **Optymalizacje numeryczne**:  
  - Dodano niewielkie przesunięcie (\(1e-10\)) do obliczeń odległości, aby uniknąć dzielenia przez zero.  
  - Dynamiczna wielkość oktantów pozwala na lepsze dostosowanie do przestrzeni zajmowanej przez ciała, minimalizując niepotrzebne podziały.

---

## Wydajność i optymalizacje
1. **Hierarchiczna dekompozycja przestrzeni**:
   - Każdy oktant reprezentuje część przestrzeni, umożliwiając obliczenia sił dla grup ciał w odległych regionach.
2. **OpenMP**:
   - Włączenie równoległości dla obliczeń sił grawitacyjnych każdego ciała.

---

## Wnioski
Projekt demonstruje:
- Skalowalność algorytmu Barnes-Hut dla dużych zbiorów danych.
- Możliwość optymalizacji obliczeń poprzez hierarchiczne modelowanie przestrzeni.
- Efektywne wykorzystanie zasobów CPU do symulacji złożonych systemów grawitacyjnych.

---

## Licencja
Projekt jest dostępny na licencji [MIT](LICENSE).
