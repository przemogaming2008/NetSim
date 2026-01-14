# NetSim
Opis projektu

Projekt NetSim jest implementacją systemu do modelowania, weryfikacji oraz symulacji działania sieci produkcyjnych, inspirowanych rzeczywistymi liniami technologicznymi w fabrykach. System umożliwia analizę przepływu półproduktów pomiędzy elementami procesu produkcyjnego oraz generowanie raportów z przebiegu symulacji.

Sieć produkcyjna jest modelowana jako spójny graf skierowany, którego węzły reprezentują elementy infrastruktury fabrycznej, a krawędzie dopuszczalne ścieżki przekazywania półproduktów.

Projekt realizowany jest w języku C++ z wykorzystaniem zasad programowania obiektowego.

# Zakres funkcjonalności

Aplikacja ma umożliwiać:

- Wczytanie struktury sieci z pliku zgodnie z określonym formatem wejściowym, błędy parsowania obsługiwane przy użyciu mechanizmu wyjątków.

- Modyfikację struktury sieci z poziomu kodu oraz zapis zmodyfikowanej sieci do pliku.

- Weryfikację poprawności sieci sprawdzenie spójności oraz poprawności połączeń.

- Symulację działania sieci krokową symulację przepływu półproduktów, generowanie raportów opisujących przebieg symulacji.

#Struktura projektu

Projekt został podzielony na logiczne moduły:

- [P] Półprodukty i ich przechowywanie
Reprezentacja półproduktów oraz mechanizmy ich składowania i przekazywania.

- [N] Węzły sieci
Implementacja ramp, robotników oraz magazynów.

- [F] Fabryka
Klasa integrująca całą sieć, zarządzająca jej strukturą i spójnością.

- [IO] Odczyt i zapis danych
Parsowanie plików wejściowych oraz zapis struktury sieci.

- [R] Raporty
Generowanie raportów ze stanu sieci i przebiegu symulacji.

- [S] Symulacja
Mechanizm czasowy symulujący działanie sieci produkcyjnej.
