#include "QWidgetListItem.h"

namespace QtAdditions
{
   using namespace std;

   QWidgetListItem* QWidgetListItem::Clone() const
   {
      return new QWidgetListItem;
   }
}

