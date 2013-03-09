#ifndef RingPlaneProjection_h
#define RingPlaneProjection_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/01/29 21:21:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include <vector>

#include "Projection.h"  
#include "PvlGroup.h" // protected data member object (m_mappingGrp) 

namespace Isis {
  class Displacement;
  class Pvl;
  class WorldMapper;
  /**
   *
   *
   * @brief Base class for Map Projections of plane shapes
   *
   * This is a virtual base class for map projections of plane shape targets. It must be used to 
   * create specific map projection classes such as PlaneCylindrical, PlanePolar, etc. The
   * foundation of this class is the ability to convert plane ground coordinates
   * (radius and azimuth) into projection coordinates (x and y) and vice
   * versa. Options exist to allow conversion to and from programmer specified
   * world coordinates. The world coordinates can be cube pixels, paper units in
   * millimeters, or any other unit the program may need. Generally, you should
   * never directly instantiate this class.
   *
   * Here is an example of how to use RingPlaneProjection
   * @code
   *   Pvl lab;
   *   lab.Read("projection.map");
   *   Projection *p = ProjectionFactory::Create(lab);
   * @endcode
   * If you would like to see Projection being used in implementation,
   * see mappos.cpp
   * @ingroup MapProjection
   * @author 2003-01-29 Jeff Anderson
   * @internal
   *   @history 2003-01-30 Jeff Anderson - Add the SetWorldMapper() method and
   *                           removed setting the mapper from the constructor
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2003-05-30 Jeff Anderson - Updated unit test to fix error for
   *                           optimization
   *   @history 2003-08-25 Jeff Anderson - Added LatitudeTypeString(),
   *                           LongitudeDirectionString(), and
   *                           LongitudeDomainString() methods
   *   @history 2003-09-26 Jeff Anderson - Added ToWorldX(), ToWorldY(),
   *                           ToProjectionX(), ToProjectionY(), and
   *                           Resolution() methods
   *   @history 2003-09-26 Jeff Anderson - Added virtual Name, operator==, and
   *                           operator!= methods
   *   @history 2003-09-26 Jeff Anderson - Remove virtual from operator!=
   *   @history 2003-10-14 Jeff Anderson - Added Scale() and TrueScaleLatitude()
   *                           methods
   *   @history 2003-11-04 Jeff Anderson - Replace the pure virtual methods for
   *                           SetGround() and SetCoordinate() with virtual
   *                           method which simply copy lat/lon to x/y and vice
   *                           versa. This is essentially no projection.
   *   @history 2003-11-04 Jeff Anderson - Added LocalRadius() methods
   *   @history 2004-02-23 Jeff Anderson - Added Eccentricity(), tCompute(),
   *                           mCompute(), e4Compute(), and phi2Compute()
   *                           methods
   *   @history 2004-02-24 Jeff Anderson - Fixed bug in eccentricity computation
   *   @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2005-02-25 Elizabeth Ribelin - Added 3 static conversion
   *                           methods: ToHours(), ToDMS(), and  ToHMS()
   *   @history 2005-03-08 Elizabeth Ribelin - Fixed unitTest
   *   @history 2005-06-24 Jeff Anderson - Tweaked format for DMS and HMS
   *                           methods
   *   @history 2005-07-22 Jeff Anderson - Added static ToPlanetographic()
   *                           method
   *   @history 2006-04-20 Tracie Sucharski - Fixed bug looking at Target
   *                           instead of TargetName for sky.
   *   @history 2006-06-20 Stuart Sides - Added ability to lookup the radii if
   *                           given a "TargetName" in the mapping group.
   *   @history 2006-07-10 Elizabeth Miller - Fixed bugs in new static method
   *   @history 2007-06-27 Steven Lambright - Added static ToPlanetocentric(),
   *                           ToPositiveWest() and ToPositiveEast() methods.
   *                           Changed To180Domain() and To360Domain() to be
   *                           static instead of const. Added Mapping(),
   *                           MappingLatitudes() and MappingLongitudes()
   *                           methods for map2map.
   *   @history 2007-08-14 Steven Lambright - Mapping() fixed: should not return
   *                           cube specific parameters, because they did not go
   *                           into making the projection.
   *   @history 2008-05-09 Steven Lambright - Added Name(), Version(),
   *                           IsEquatorialCylindrical() methods
   *   @history 2008-06-12 Christopher Austin - Elaborated error messages.
   *   @history 2008-06-19 Steven Lambright - Fixed memory leak
   *   @history 2009-01-29 Stacy Alley - added a overloaded STATIC
   *                           method for convenience.  TargetRadii, which takes
   *                           a Pvl, the cube label, and a PvlGroup, a mapping
   *                           group.
   *   @history 2011-02-10 Jai Rideout - added SetUpperLeftCorner() because
   *                           ProjectionFactory needed a way to set
   *                           UpperLeftCornerX and UpperLeftCornerY keywords
   *                           after creating the projection. Mapping() now adds
   *                           optional keywords UpperLeftCornerX,
   *                           UpperLeftCornerY, PixelResolution, and Scale to
   *                           original mapping group so that cam2map can
   *                           properly display a clean mapping group.
   *   @history 2011-07-05 Jeannie Backer - Added qCompute() method. Updated
   *                           documentation.
   *   @history 2011-08-11 Steven Lambright - phi2Compute was running out of
   *                           iterations for vesta (an asteroid). Fixes #279
   *   @history 2012-03-01 Jeff Anderson - Fixed bug in SetUpperLeftCorner by
   *                           adding Pvl::Replace when updating the mapping
   *                           labels 
   *   @history 2012-03-30 Steven Lambright and Stuart Sides - To360Domain() and
   *                           To180Domain() are now constant time operations.
   *                           Fixes #656.
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declarations to header file. Ordered includes in
   *                           implementation file. Moved destructor and
   *                           accessor methods to the implementation file.
   *                           Moved the following methods from the
   *                           ObliqueCylindrical class for
   *                           generalized xyRangeOblique() method - doSearch(),
   *                           findExtreme(), setSearchGround(). Changed member
   *                           variable prefix to m_. Improved unitTest.
   *                           Resolves #928.
   *   @history 2012-07-25 Jeannie Backer - Modified the new methods related to
   *                           xyRangeOblique() to verify the validity of the
   *                           values when setSearchGround() is called.
   *                           References #954.
   *   @history 2012-08-01 Kimberly Oyama - Added a qFuzzyCompare() to ToPlanetographic() to
   *                           account for rounding error at the latitude boundaries (-90 and
   *                           90 degrees). Updated the unit test to exercise this change.
   *                           References #604.
   *   @history 2012-08-09 Steven Lambright - Added NaifStatus calls to protect
   *                           the TargetRadii() method from naif throwing an
   *                           error/crashing with inputs that have non-target naif codes.
   *   @history 2012-11-23 Debbie A. Cook - New RingPlaneProjection child/base class
   *  
   *   @todo Continue to modify Projection class to comply with coding
   *         standards. Some of these include, but may not be limited to remove
   *         "Get" from methods GetX and GetY, change methods to lower camel
   *         case, add methods for RelativeScaleFactorLatitude and
   *         RelativeScaleFactorLongitude (see LambertAzimuthalEqualArea.cpp).
   */
  class RingPlaneProjection : public Projection {
    public:
      // constructor
      RingPlaneProjection(Pvl &label);
      virtual ~RingPlaneProjection();
      virtual bool operator== (const Projection &proj);

      // These return properties of the target
      double LocalRadius() const; // requires SetGround or SetCoordinate

      // These return or change properties of the projection, independent of calculations
      /**
       * This method returns the name of the map projection.  It is a pure 
       * virtual method (requires all subclasses to override). 
       *
       * @return string The name of the map projection.
       */
      virtual QString Name() const = 0;
      /**
       * This method returns the Version of the map projection.  It is a pure 
       * virtual method (requires all subclasses to override).
       *
       * @return string The Version number of the map projection.
       */
      virtual QString Version() const = 0;
      virtual double TrueScaleRadius() const;

      /**
       * This enum defines the types of Azimuth directions supported in this 
       * class.  
       */
      enum AzimuthDirection { Clockwise, /**< Azimuth values increase in 
                                                   the clockwise direction. */
                                CounterClockwise  /**< Azimuth values increase in 
                                                   the counterclockwise direction.*/
                              };

      // Check azimuth direction or get azimuth direction as a string
      bool IsClockwise() const;
      bool IsCounterClockwise() const;
      std::string AzimuthDirectionString() const;

      // Check azimuth domain or get azimuth domain as a string  TODO** check implementation to see if this can go in Projection
      bool Has180Domain() const;
      bool Has360Domain() const;
      std::string AzimuthDomainString() const;

      // Get min/max rad/az
      double MinimumRadius() const;
      double MaximumRadius() const;
      double MinimumAzimuth() const;
      double MaximumAzimuth() const;

      //  Calculations
      // Set ground position or x/y coordinate
      virtual bool SetGround(const double rad, const double az);
      virtual bool SetCoordinate(const double x, const double y);

      // Methods that depend on successful completion
      // of SetGround/SetCoordinate Get rad,az, x,y
      double Radius() const;
      double Azimuth() const;

      // Set the universal ground coordinate (calls SetGround)
      bool SetUniversalGround(const double rad, const double az);

      // Return the universal ground coordinate after successful SetCoordinate
      double UniversalRadius();
      double UniversalAzimuth();

      // convert from world coordinate to projected coordinate
      //      double ToProjectionX(const double worldX) const;
      //      double ToProjectionY(const double worldY) const;

      // get resolution and scale for mapping world coordinates
      double Scale() const;

      // Return the x/y range which covers the rad/az range in the labels
      virtual bool XYRange(double &minX, double &maxX, 
                           double &minY, double &maxY);

      // get mapping information
      virtual PvlGroup Mapping();
      virtual PvlGroup MappingRadii();
      virtual PvlGroup MappingAzimuths();

      // change azimuth direction
      static double ToClockwise(const double az, const int domain);
      static double ToCounterClockwise(const double az, const int domain);

      // change azimuth domain 
      static double To180Domain(const double lon);
      static double To360Domain(const double lon);

    protected:
      void XYRangeCheck(const double radius, const double azimuth);
      //      bool xyRangeOblique(double &minX, double &maxX, 
      //                          double &minY, double &maxY);

    private:
      // These methods are not currectly being used by the ring plane projections
      /*      void doSearch(double minBorder, double maxBorder, 
                    double &extremeVal, const double constBorder,
                    bool searchX, bool searchAzimuth, bool findMin);
      void findExtreme(double &minBorder,  double &maxBorder, 
                       double &minBorderX, double &minBorderY, 
                       double &maxBorderX, double &maxBorderY, 
                       const double constBorder, bool searchX, 
                       bool searchAzimuth, bool findMin);
      void setSearchGround(const double variableBorder, 
                           const double constBorder, bool variableIsRad);
      */
    protected:
      double m_radius;   /**< This contain a radius value. The value is 
                                only usable if m_good is true.*/
      // 1.  The horizontal angular distance from a reference direction, usually the northern point 
      //      of the horizon, to the point where a vertical circle through a celestial body 
      //      intersects the horizon, usually measured clockwise. Sometimes the southern point
      //      is used as the reference direction, and the measurement is made clockwise through 360°.
      // 2.  The horizontal angle of the observer's bearing in surveying, measured clockwise from a 
      //      referent direction, as from the north, or from a referent celestial body, usually Polaris.
      double m_azimuth;  /**< This contain a azimuth value. The value is 
                                only usable if m_good is true.*/


      AzimuthDirection m_azimuthDirection; /**< An enumerated type indicating the 
                                                    LongitudeDirection read from the 
                                                    labels. It can be either PositiveEast
                                                    or PositiveWest. Indicating which 
                                                    direction the positive axis for 
                                                    longitude is.**/

      // TODO** Can this be generalized for both longitude and azimuth???
      int m_azimuthDomain; /**< This integer is either 180 or 360 and is read 
                                  from the labels. It represents the azimuth 
                                  domain when returning values through Azimuth
                                  method. The domain is either -180 to 180 or
                                  0 to 360.**/

      double m_minimumRadius;   /**< Contains the minimum radius for the
                                       entire ground range. Only usable if
                                       m_groundRangeGood is true.*/
      double m_maximumRadius;   /**< Contains the maximum radius for the
                                       entire ground range. Only usable if
                                       m_groundRangeGood is true.*/
      double m_minimumAzimuth;  /**< Contains the minimum longitude for the
                                       entire ground range. Only usable if
                                       m_groundRangeGood is true.*/
      double m_maximumAzimuth;  /**< Contains the maximum longitude for the
                                       entire ground range. Only usable if
                                       m_groundRangeGood is true.*/

    private:
  };
};
#endif

