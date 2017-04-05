#ifndef OpenRecentProjectWorkOrder_H
#define OpenRecentProjectWorkOrder_H
/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "WorkOrder.h"

class QString;

namespace Isis {


  /**
   *  @brief  This is a child of class WorkOrder which is used for anything that performs
   *  an action in a Project.  This class opens and displays a recent
   *  project.
   *
   * @author 2014-04-13 Ken Edmundson
   *
   *
   * @internal
   *   @author 2016-06-06 Tyler Wilson - Added documentation for the functions and
   *              brought the code into compliance with ISIS3 coding standards.
   *              References #3944.
   */


  class OpenRecentProjectWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      OpenRecentProjectWorkOrder(Project *project);
      OpenRecentProjectWorkOrder(const OpenRecentProjectWorkOrder &other);
      ~OpenRecentProjectWorkOrder();

      virtual OpenRecentProjectWorkOrder *clone() const;

      virtual bool isExecutable(QString projectFileName);
      bool setupExecution();

    private:
      OpenRecentProjectWorkOrder &operator=(const OpenRecentProjectWorkOrder &rhs);
  };
}

#endif // OpenRecentProjectWorkOrder_H
