import numpy as np
import matplotlib.pyplot as plt


def check_multiply() -> None:
    """
    Функция для верификации результатов вычислений

    :return: None
    """
    for size in [100, 250, 500, 750, 1000]:
        if np.array_equal(np.dot(*[np.loadtxt(f'data/matrix_{name}_{size}.txt') \
            for name in ['A', 'B']]), np.loadtxt(f'data/res_AxB_{size}.txt')):
            print(f'Размеры {size} совпадают!')
        else:
            print(f'Размеры {size} не совпадают!')


def draw_graph() -> None:
    """
    Функция для построения графика

    :return: None
    """
    with open('data/avg_times.txt', 'r') as file:
        sizes, times = zip(*[map(int, line.strip().split('-')) for line in file])
    plt.plot(sizes, times, marker='o', linestyle='-')
    plt.title('Среднее время')
    plt.xlabel('Размер матрицы')
    plt.ylabel('Время (мс)')
    plt.grid(True)
    plt.show()


if __name__ == '__main__':
    check_multiply()
    draw_graph()