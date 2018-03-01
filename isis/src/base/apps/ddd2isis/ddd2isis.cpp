#include "Isis.h"
#include "ProcessImport.h"
#include "UserInterface.h"
#include "SpecialPixel.h"
#include "FileName.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  IString from = ui.GetFileName("FROM");
  ifstream fin;

  fin.open(from.c_str(), ios::in | ios::binary);
  if( !fin.is_open() ) {
    string msg = "Cannot open input file [" + from + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  /**
   *  0-rel byte offset   value
   *       0          32-bit integer magic number
   *       4          32-bit integer number of image lines
   *       8          32-bit integer number of bytes per image line
   *      12          32-bit integer number of bits per image elements
   *      16          32-bit integer currently unused
   *      20          32-bit integer number of bytes to start of image data
   *      24          ASCII label up to 1000 characters long
   *                  The label is NUL-terminated
   *
   */

   // ifstream read() needs a char* to read values into, so the union
   // is used to store read values
   union {
     char readChars[4];
     long readLong;
     float readFloat;
   } readBytes;

   // ddd files are LSB
   EndianSwapper swp("MSB");

  // Verify that the file is a ddd by reading in the first 4 bytes and
  // comparing the magic numbers
  readBytes.readLong = 0;
  fin.seekg(0);
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);

  if(readBytes.readLong != 1659) {
    string msg = "Input file [" + from + "] does not appear to be in ddd format";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  // Read bytes 4-7 to get number of lines
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);
  int nLines = (int) readBytes.readLong;

  // Read bytes 8-11 to get number of bytes
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);
  int nBytes = (int) readBytes.readLong;

  // Read bytes 12-15 to get the total number of bits out of all the bands
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);

  if( fin.fail() || fin.eof() ) {
    string msg = "An error ocurred when reading the input file [" + from + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
  int totalBandBits = readBytes.readLong;

  // Maps the bit type of the file to the number of bytes of that type
  map<int, int> dataTypes = {
    {1450901768, 1},
    {1450902032, 2},
    {1450902288, 2},
    {1450902560, 4},
    {1450902816, 4},
    {1450903072, 4},
    {1450903360, 8},
    {8, 1},
    {16, 2},
    {48, 2}
  };

  // Read bytes 16-19 to get the bit type
  // Map the bit type to the number of bytes and store in dataTypeBytes
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);
  int bitType = (int) readBytes.readLong;

  int dataTypeBytes;
  //Old header format has no bit type
  if (bitType == 0) {
    dataTypeBytes = dataTypes.find(totalBandBits) -> second;
  }
  else {
    dataTypeBytes = dataTypes.find(bitType) -> second;
  }

  // Read bytes 20-23 to get offset
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);
  int nOffset = (int) readBytes.readLong;
  if (nOffset < 1024) {
    nOffset = 1024;
  }

  PvlGroup results("FileInfo");
  results += PvlKeyword( "NumberOfLines", toString(nLines) );
  results += PvlKeyword( "NumberOfBytesPerLine", toString(nBytes) );
  results += PvlKeyword( "BitType", toString(totalBandBits) );
  int nSamples = nBytes / (totalBandBits / 8);
  results += PvlKeyword( "NumberOfSamples", toString(nSamples) );
  int nBands = (totalBandBits / 8) / dataTypeBytes;
  results += PvlKeyword( "NumberOfBands", toString(nBands) );
  results += PvlKeyword( "LabelBytes", toString(nOffset) );
  Application::Log(results);

  fin.close();

  ProcessImport p;

  int bitsPerBand = totalBandBits / nBands;
  if ( ui.WasEntered("TO") ) {
    switch(bitsPerBand) {
      case 8:
        p.SetPixelType(Isis::UnsignedByte);
        break;
      case 16:
        p.SetPixelType(Isis::UnsignedWord);
        break;
      case 32:
        p.SetPixelType(Isis::Real);
        break;
      default:
        IString msg = "Unsupported bit per pixel count [" + IString(totalBandBits) + "]. "; //Do we need this?
        msg += "(Use the raw2isis and crop programs to import the file in case it is ";
        msg += "line or sample interleaved.)";
        throw IException(IException::Io, msg, _FILEINFO_);
    }

    // ddd files with more than one band are pixel interleaved
    // Having one band is similar to BIP, but this is here for clarification
    if (nBands > 1) {
      p.SetOrganization(ProcessImport::BIP);
    }

    p.SetDimensions(nSamples, nLines, nBands);
    p.SetFileHeaderBytes(nOffset);
    p.SetByteOrder(Isis::Msb);
    p.SetInputFile( ui.GetFileName("FROM") );
    p.SetOutputCube("TO");

    p.StartProcess();
    p.EndProcess();
  }
}
