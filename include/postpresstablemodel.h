#ifndef POSTPRESSTABLEMODEL_H
#define POSTPRESSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include "typography.h"


class PostpressTableModel : public QAbstractTableModel
{
public:
    PostpressTableModel(std::vector<PasportBase::Postpress> &ppress, QObject *parent = nullptr);

private:
    std::vector <PasportBase::Postpress> &postpress;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual bool insertRows(int id,
                            QString name,
                            double price,
                            int cnt,
                            int row = 0,
                            int count = 0,
                            const QModelIndex &parent = QModelIndex());


    // QAbstractItemModel interface
public:
    virtual bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) override;
    void setPostpress(std::vector<PasportBase::Postpress> &ppress)
    {postpress = ppress;}
};

#endif // POSTPRESSTABLEMODEL_H
