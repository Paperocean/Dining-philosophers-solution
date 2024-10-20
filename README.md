# Problem Filozofów przy obiedzie

Ten kod w C++ implementuje symulację problemu filozofów przy obiedzie, wykorzystując wątki i mechanizmy synchronizacji.

Problem filozofów przy obiedzie to klasyczny problem synchronizacji, który ilustruje wyzwania związane z unikaniem zakleszczeń (deadlock) i rywalizacji o zasoby (contention) w systemie wieloprocesowym.
Chociaż użycie blokad (locks) dla poszczególnych widelców może rozwiązać ten problem, uznałem, że implementacja blokad dla obu widelców jednocześnie wydaje się być wystarczającym rozwiązaniem, aby złagodzić problem.

## Funkcje

- **Synchronizacja za pomocą wątków**: Każdy filozof to osobny wątek, który próbuje zdobyć dwa widelce.
- **Unikanie zakleszczeń**: Implementacja zapewnia mechanizmy zapobiegające zakleszczeniu filozofów, gdy wszyscy próbują jeść w tym samym czasie.

## Opis Problemu
Filozofowie siedzą przy okrągłym stole. Każdy z nich potrzebuje dwóch widelców (po jednym z lewej i z prawej strony), aby zjeść. Problem polega na tym, że gdy wszyscy filozofowie jednocześnie próbują podnieść widelce, może dojść do zakleszczenia, gdzie żaden z filozofów nie jest w stanie jeść.
