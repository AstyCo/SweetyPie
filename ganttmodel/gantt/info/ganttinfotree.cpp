#include "ganttinfotree.h"
#include "gantt/private_extensions/gantt-lib_global_values.h"

GanttInfoTree::GanttInfoTree(QObject *parent) : QObject(parent)
{
    init();
}

void GanttInfoTree::setModel(QAbstractItemModel *model)
{
    if(!model)
        return;

    disconnectLastModel();
    _model = model;
    connectNewModel();

    reset();
}

void GanttInfoTree::setBuilder(AbstractBuilder *builder)
{
    _builder = builder;

    if(_builder)
        setModel(_builder->model());
}

void GanttInfoTree::connectTreeView(QTreeView *view)
{
    if(!view)
        return;

    connect(this,SIGNAL(needCollapse(QModelIndex)),view,SLOT(collapse(QModelIndex)));
    connect(this,SIGNAL(needExpand(QModelIndex)),view,SLOT(expand(QModelIndex)));
    connect(this,SIGNAL(currentChanged(QModelIndex, QItemSelectionModel::SelectionFlags)),
            view->selectionModel(),SLOT(setCurrentIndex(QModelIndex, QItemSelectionModel::SelectionFlags)));

    connect(view,SIGNAL(expanded(QModelIndex)),this,SLOT(onExpanded(QModelIndex)));
    connect(view,SIGNAL(collapsed(QModelIndex)),this,SLOT(onCollapsed(QModelIndex)));
    connect(view,SIGNAL(clicked(QModelIndex)),this,SLOT(onClicked(QModelIndex)));

}

void GanttInfoTree::disconnectTreeView(QTreeView *view)
{
    if(view){
        view->disconnect(this);
        disconnect(view);
    }
}

GanttInfoNode *GanttInfoTree::root() const
{
    return _root;
}

GanttInfoItem *GanttInfoTree::infoForVPos(int vpos)
{
    return lookupForVPos(vpos,_root);
}

int GanttInfoTree::height() const
{
    return heightH(_root);
}

void GanttInfoTree::onCurrentItemChanged(const GanttInfoItem *item)
{
    if(!item)
        return;
    qDebug() << "emit currentChanged" << item->index();
    emit currentChanged(item->index(), QItemSelectionModel::Select | QItemSelectionModel::Current);
}

GanttInfoItem *GanttInfoTree::infoForIndex(const QModelIndex &index) const
{
    if(!index.isValid())
        return _root;
    return _infoForIndex.value(index, NULL);    // NULL default value
}

GanttInfoItem *GanttInfoTree::nextStart(const UtcDateTime &dt) const
{
    if(dt.isValid())
    {
        QMap<UtcDateTime,GanttInfoItem*>::const_iterator it = _infoForStart.constBegin();
        while (it != _infoForStart.constEnd())
        {
            if(dt < it.key())
                return it.value();
            ++it;
        }
    }

    return NULL;
}

GanttInfoItem *GanttInfoTree::prevFinish(const UtcDateTime &dt) const
{
    if(dt.isValid())
    {
        QMapIterator<UtcDateTime,GanttInfoItem*> it(_infoForFinish);
        it.toBack();
        while (it.hasPrevious()) {
            it.previous();
            if(dt > it.key())
                return it.value();
        }
    }

    return NULL;
}

void GanttInfoTree::onClicked(const QModelIndex &index)
{
    qDebug() << "currentChanged" << index;
    emit currentChanged(infoForIndex(index.sibling(index.row(),0)));
}

void GanttInfoTree::onExpanded(const QModelIndex &index)
{
    GanttInfoNode *node = dynamic_cast<GanttInfoNode*>(infoForIndex(index));
    if(node)
        node->setExpanded(true);
}

void GanttInfoTree::onCollapsed(const QModelIndex &index)
{
    GanttInfoNode *node = dynamic_cast<GanttInfoNode*>(infoForIndex(index));
    if(node)
        node->setExpanded(false);
}

void deleteInfoItemFunc(GanttInfoItem* item)
{
    if(item->parent())
        item->deleteInfoItem();
}

void GanttInfoTree::clear()
{
    if(_root)
        _root->callForEachItemRecursively(&deleteInfoItemFunc);
    _root->clear();

    clearLimits();

    _infoForIndex.clear();  // clears cache
    _infoForStart.clear();  // clears cache
    _infoForFinish.clear(); // clears cache

    emit endRemoveItems();
}

void GanttInfoTree::reset()
{
    if(!_model){
        qWarning("GanttInfoTree called reset w/o model");
        return;
    }

    clear();
    fillByModelIndex();

    emit treeReset();
    onAnyAddition();
}

void GanttInfoTree::onNodeExpanded()
{
    GanttInfoNode *node = qobject_cast<GanttInfoNode*>(sender());
    if(!node)
        return;
    emit expanded(node);
    emit needExpand(node->index());
}

void GanttInfoTree::onNodeCollapsed()
{
    GanttInfoNode *node = qobject_cast<GanttInfoNode*>(sender());
    if(!node)
        return;
    emit collapsed(node);
    emit needCollapse(node->index());
}

void GanttInfoTree::onDataChanged(const QModelIndex &/*from*/, const QModelIndex &/*to*/)
{
    qDebug() << "onDataChanged";
    /// TODO optimization
    reset();
}

void GanttInfoTree::onRowsInserted(const QModelIndex &parent, int start, int end)
{
//    qDebug() << "onRowsIns " << start << " " << end;
    fill(qobject_cast<GanttInfoNode*>(infoForIndex(parent)), parent, start, end);

    onAnyAddition();
}

void GanttInfoTree::onColumnsInserted(const QModelIndex &/*parent*/, int /*start*/, int /*end*/)
{
    /// TODO optimization
    reset();
}

void GanttInfoTree::onRowsRemoved(const QModelIndex &/*parent*/, int /*start*/, int /*end*/)
{
    /// TODO optimization
    reset();

    clearLimits();
    updateLimits();
    emitLimitsChanged();
}

void GanttInfoTree::onColumnsRemoved(const QModelIndex &/*parent*/, int /*start*/, int /*end*/)
{
    /// TODO optimization
    reset();
}

void GanttInfoTree::onItemAboutToBeDeleted()
{
    GanttInfoItem *item = qobject_cast<GanttInfoItem*>(QObject::sender());

    _infoForIndex.remove(item->index());    // clears cache

    if(_builder && _builder->isEvent(item)){
        clearInfoForDtHelper(_infoForStart, item->start(), item);
        clearInfoForDtHelper(_infoForFinish, item->finish(), item);
    }

    if(item)
        emit itemAboutToBeDeleted(item);
}

void GanttInfoTree::updateLimits()
{
    updateLimitsByItem(_root);
}

GanttInfoItem *GanttInfoTree::lookupForVPos(int vpos, GanttInfoNode *node)
{
    GanttInfoItem *foundItem = NULL;
    if(vpos < node->pos())
        return NULL;
    if((!node->isExpanded() && vpos < node->pos() + node->height())
        || (node->size()>0 && vpos < node->at(0)->pos()) )
        return node;

    for(int i = 0; i < node->size(); ++i)
        if(vpos < node->at(i)->pos() + node->at(i)->height() ){
            foundItem = node->at(i);   // found
            break;
        }

    if(foundItem){
        GanttInfoNode *p_node = qobject_cast<GanttInfoNode*>(foundItem);
        if(p_node){
            if(p_node->size() == 0)
                return p_node;
            else
                return lookupForVPos(vpos, p_node);
        }
        return foundItem;
    }

    return NULL; // not found
}

void GanttInfoTree::fill(GanttInfoNode *node, const QModelIndex &index, int from, int to)
{
    if(!_model){
        qWarning("fill called reset w/o model");
        return;
    }
    for(int i = from; i <= to; ++i){
        QModelIndex childIndex = _model->index(i,0,index);
        GanttInfoItem *childItem = makeInfoItem(childIndex);
        node->append(childItem);
    }
    emit rowsInserted(node, from, to);
}

GanttInfoItem *GanttInfoTree::makeInfoItem(const QModelIndex &index)
{
    if(!_builder){
        qWarning("makeInfoItem called reset w/o builder");
        return NULL;
    }
    GanttInfoItem *item = _builder->createInfo(index);
//    qDebug() << "for index" << index << "createdInfo"  <<item->title() << item;
    if(item){
        _infoForIndex.insert(index, item);  // fills cache

        if(_builder->isEvent(item)){
            _infoForStart.insert(item->start(), item);      // fills cache
            _infoForFinish.insert(item->finish(), item);    // fills cache
        }

        emit itemAdded(item);
    }
    return item;
}

void GanttInfoTree::init()
{
    _limitsChanged = false;
    _model = NULL;
    _builder = NULL;

    _root = new GanttInfoNode(this);
    _root->setExpanded(true);
    _root->setTitle(QString("GanttInfoTree_ROOT"));

    connect(this,SIGNAL(itemAdded(GanttInfoItem*)),this,SLOT(connectNewItem(GanttInfoItem*)));
    connect(this,SIGNAL(itemAdded(GanttInfoItem*)),this,SLOT(updateLimitsByItem(GanttInfoItem*)));
}

void GanttInfoTree::disconnectLastModel()
{
    if(_model)
        _model->disconnect(this);
}

void GanttInfoTree::connectNewModel()
{
    connect(_model, SIGNAL(modelReset()),
            this,SLOT(reset()));

    connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
    connect(_model, SIGNAL(columnsInserted(QModelIndex,int,int)),
            this, SLOT(onColumnsInserted(QModelIndex,int,int)));
    connect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(onRowsInserted(QModelIndex,int,int)));

    connect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(onRowsRemoved(QModelIndex,int,int)));
    connect(_model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this, SLOT(onColumnsRemoved(QModelIndex,int,int)));
}

void GanttInfoTree::clearInfoForDtHelper(QMap<UtcDateTime, GanttInfoItem *> &map, const UtcDateTime &dt, GanttInfoItem *item)
{
    QMap<UtcDateTime,GanttInfoItem*>::iterator i = map.find(dt);
    while (i != map.end() && i.key() == dt){
        if(i.value() == item){
            map.erase(i);
            break;
        }
        ++i;
    }
}

void GanttInfoTree::connectNewItem(GanttInfoItem *item)
{
    connect(item,SIGNAL(aboutToBeDeleted()),this,SLOT(onItemAboutToBeDeleted()));

    if(GanttInfoNode *node = qobject_cast<GanttInfoNode*>(item)){
        connect(node,SIGNAL(expanded()),this,SLOT(onNodeExpanded()));
        connect(node,SIGNAL(collapsed()),this,SLOT(onNodeCollapsed()));
    }

}

void GanttInfoTree::updateLimitsByItem(GanttInfoItem *item)
{
    QPair<UtcDateTime, UtcDateTime> tmp = GanttInfoItem::myMax(_limits, item->getLimits());
    if(_limits != tmp)
        setLimits(tmp);
}

void GanttInfoTree::emitLimitsChanged()
{
    if(_limitsChanged){
        emit limitsChanged(_limits.first,_limits.second - _limits.first);
        _limitsChanged = false;
    }
}

void GanttInfoTree::collapseAll()
{
    for(int i = 0; i<_root->size() ; ++i)
    {
        GanttInfoNode *node = _root->nodeAt(i);
        if(node && node->isExpanded())
        {
            node->setExpanded(false);
        }
    }
}

void GanttInfoTree::clearLimits()
{
    _limits.first = UtcDateTime();      // clears limits
    _limits.second = UtcDateTime();     // clears limits
    emitLimitsChanged();
}
void GanttInfoTree::fillByModelIndex(const QModelIndex &parent)
{
//    qDebug() << "fillByModelIndex " << parent;
    if((!parent.isValid() || _model->canFetchMore(parent)) && _model->hasChildren(parent)){
//        qDebug() << "fill " << parent << _model->rowCount(parent)-1;
        fill(static_cast<GanttInfoNode*>(infoForIndex(parent)),
             parent, 0, _model->rowCount(parent) - 1);

        for(int i=0; i < _model->rowCount(parent); ++i)
            fillByModelIndex(_model->index(i, 0, parent));
    }
}

void GanttInfoTree::setLimits(const QPair<UtcDateTime, UtcDateTime> &newLimits)
{
    _limits = newLimits;
    _limitsChanged = true;
}

int GanttInfoTree::heightH(GanttInfoItem *item) const
{
    if(GanttInfoNode *node = qobject_cast<GanttInfoNode*>(item)){
        if(!node->isEmpty())
            return heightH(node->at(node->size() - 1));
    }
    return item->pos() + DEFAULT_ITEM_HEIGHT * 1.5;
}

void GanttInfoTree::onAnyAddition()
{
    collapseAll();
    emitLimitsChanged();
}

QAbstractItemModel *GanttInfoTree::model() const
{
    return _model;
}
