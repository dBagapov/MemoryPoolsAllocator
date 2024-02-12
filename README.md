# MemoryPoolsAllocator

Одна из моих лабораторных работ с 1 курса

## Задача

Реализовать аллокатор, работающий на основе выделенных заранее блоков памяти, соответсвующий [требованиям к аллокаторам](https://en.cppreference.com/w/cpp/named_req/Allocator).

Произвести сравнение с стандарным аллокатором при взаимодействии с контейнерами, полученные результаты изобразить на графике. Например, потраченное время на добавление элемента в вектор, в зависимости от количества добавленных элементов.

Данное исследование также является частью задания и повлияет на итоговый балл.

## Принцип работы

- Для аллокации памяти аллокатор использует заранее выделенные участки памяти
- Память изначальльно выделяется [пулами](https://en.wikipedia.org/wiki/Memory_pool)
- Пулы бьются на кусочки одинакового размера
- Пул умеет выделять и освобождать память только кусочками (один или более)
- Пул обладает информацией о свободных и занятых кусочках
- Пул очищает память лишь в конце своей "жизни"
- Аллокатор при выделение памяти размер N байт ищет среди пулов наиболее подходящий так, чтобы наиболее оптимально выделять память, если это невозможно то выбрасывается исключение std::bad_alloc
- При освобождении через аллокатор, кусочки памяти вновь пригодны для использования


## Реализация

Размеры пулов, их количество, а также размеры кусочков - параметризуются (в compile time или runtime).
Аллокатор не обязательно должен владеть пулами. Дизайн на Ваше усмотрение.
