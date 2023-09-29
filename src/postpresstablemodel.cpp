#include "include/postpresstablemodel.h"

PostpressTableModel::PostpressTableModel(std::vector<PasportBase::Postpress> &ppress,
                                         QObject *parent)
    : QAbstractTableModel(parent), postpress(ppress)
{

}

int PostpressTableModel::rowCount(const QModelIndex &parent) const
{
    return postpress.size();
}

int PostpressTableModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

QVariant PostpressTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= int(postpress.size()))
        return QVariant();

    if (role == Qt::DisplayRole)
        switch (index.column())
        {
        case 0:
            return postpress[index.row()].id;
            break;
        case 1:
            return postpress[index.row()].name;
            break;
        case 2:
            return postpress[index.row()].price;
            break;
        case 3:
            return postpress[index.row()].count;
            break;
        case 4:
            return postpress[index.row()].cost;
            break;
        default:
            break;
        }
    return QVariant();
}

bool PostpressTableModel::setData(const QModelIndex &index,
                                  const QVariant &value,
                                  int role)
{
    if (!index.isValid())
        return false;

    switch (index.column())
    {
    case 0:
        postpress[index.row()].id = value.toInt();
        break;
    case 1:
        postpress[index.row()].name = value.toString();
        break;
    case 2:
        postpress[index.row()].price = value.toDouble();
        break;
    case 3:
        postpress[index.row()].count = value.toInt();
        break;
    case 4:
        postpress[index.row()].cost = value.toDouble();
        break;
    default:
        break;
    }
    return false;
}

QVariant PostpressTableModel::headerData(int section,
                                         Qt::Orientation orientation,
                                         int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        switch (section)
        {
        case 0:
            return QString("ID");
            break;
        case 1:
            return QString("Вид работ");
            break;
        case 2:
            return QString("Цена");
            break;
        case 3:
            return QString("Кол-во");
            break;
        case 4:
            return QString("Стоимость \n руб.");
            break;
        default:
            break;
        }

    return QVariant();
}

bool PostpressTableModel::insertRows(int id,
                                     QString name,
                                     double price,
                                     int cnt,
                                     int row,
                                     int count,
                                     const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, count);

    PasportBase::Postpress p;
    p.id = id;
    p.name = name;
    p.price = price;
    p.count = cnt;
    p.cost = price * cnt;

    postpress.push_back(p);

    endInsertRows();
    return true;
}

bool PostpressTableModel::removeRows(int row,
                                     int count,
                                     const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    postpress.erase(postpress.begin() + row);
    endRemoveRows();
    return true;
}
