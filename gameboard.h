#pragma once
#include <QAbstractListModel>
#include <deque>
#include <vector>
#include <QColor>

class GameBoard : public QAbstractListModel
{
    Q_OBJECT

public:
    using Position = std::pair<size_t, size_t>;
    using IteratorPosition = std::pair<std::deque<std::deque<QColor>>::iterator, std::deque<QColor>::iterator>;

    static constexpr size_t defaultPuzzleDimension = 8;

    GameBoard(QObject *parent = nullptr, size_t board_dimension = defaultPuzzleDimension);

    int rowCount(const QModelIndex &parent = QModelIndex {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DecorationRole) const override;

    Q_INVOKABLE void shuffle();

    void generateBoard();
    bool generationCheck();
    void removeMarkedTiles();
    void addNewTiles();
    void switchTiles(int indexFrom, int indexTo);
    Q_INVOKABLE void moveMade(int indexFrom, int indexTo);

    Position getRowCol(const size_t index) const;
    int getIndex(const Position position) const;

private:
    QColor getRandomColor();
    bool isAdjacent(const Position f, const Position s);

    const size_t m_dimension;
    std::mt19937 generator;
    const std::vector<QColor> m_colors {QColor("red"), QColor("green"),QColor("blue"),
                                        QColor("violet"), QColor("yellow")};
    std::uniform_int_distribution<int> randomColor;
    QList<QList<QColor>> m_board;
    std::deque<Position> m_markedTiles;
};
