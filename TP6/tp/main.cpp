#include <igl/readOFF.h>
#include <igl/opengl/glfw/Viewer.h>
#include <bitset>

using namespace std;

Eigen::Vector3d GravityCenter(const Eigen::MatrixXd& _vertices)
{
  Eigen::Vector3d _center = Eigen::Vector3d(0,0,0);
  int _nbVertices = _vertices.rows();
  for(int i = 0; i < _nbVertices; ++i)
    _center += _vertices.row(i);
  return _center/(float)_nbVertices;
}

Eigen::Vector3d CartesianToSpherical(const Eigen::Vector3d& _cartesian, const Eigen::Vector3d& _center)
{
  float _rho = sqrt((_cartesian(0) - _center(0))*(_cartesian(0) - _center(0)) 
                  + (_cartesian(1) - _center(1))*(_cartesian(1) - _center(1)) 
                  + (_cartesian(2) - _center(2))*(_cartesian(2) - _center(2)));
  float _theta = atan2((_cartesian(1) - _center(1)), (_cartesian(0) - _center(0)));
  float _phi = acos((_cartesian(2) - _center(2))/_rho);
  return Eigen::Vector3d(_rho, _theta, _phi);
}

Eigen::Vector3d SphericalToCartesian(const Eigen::Vector3d& _spherical, const Eigen::Vector3d& _center)
{
  float _x = _spherical(0)*cos(_spherical(1))*sin(_spherical(2)) + _center(0);
  float _y = _spherical(0)*sin(_spherical(1))*sin(_spherical(2)) + _center(1);
  float _z = _spherical(0)*cos(_spherical(2)) + _center(2);
  return Eigen::Vector3d(_x, _y, _z);
}

Eigen::Vector2d MinMaxCoordRadiales(const vector<Eigen::Vector4d>& _vertices)
{
  float _min = numeric_limits<float>::max();
  float _max = numeric_limits<float>::min();
  for(int i = 0; i < _vertices.size(); ++i)
  {
    _min = min(_min, (float)_vertices[i](0));
    _max = max(_max, (float)_vertices[i](0));
  }
  return Eigen::Vector2d(_min, _max);
}

vector<Eigen::Vector4d> Normalize(const vector<Eigen::Vector4d>& _vertices)
{

  Eigen::Vector2d _min_max = MinMaxCoordRadiales(_vertices);

  vector<Eigen::Vector4d> _normalizedValues = vector<Eigen::Vector4d>(_vertices.size());
  for(int i = 0; i < _vertices.size(); ++i)
  {
    float _rhoNormalized = (_vertices[i](0) - _min_max(0))/(_min_max(1)-_min_max(0));
    _normalizedValues[i] = Eigen::Vector4d(_rhoNormalized, _vertices[i](1), _vertices[i](2), _vertices[i](3));
  }
  return _normalizedValues;
}

vector<Eigen::Vector4d> UnNormalize(const vector<Eigen::Vector4d>& _vertices, float _min, float _max)
{
  vector<Eigen::Vector4d> _unNormalizedValues = vector<Eigen::Vector4d>(_vertices.size());
  for(int i = 0; i < _vertices.size(); ++i)
  {
    float _rhoUnNormalized = _vertices[i](0) *(_max-_min) + _min;
    _unNormalizedValues[i] = Eigen::Vector4d(_rhoUnNormalized, _vertices[i](1), _vertices[i](2), _vertices[i](3));
  }
  return _unNormalizedValues;
}

vector<vector<Eigen::Vector4d>> SortInBins(const vector<Eigen::Vector4d>& _vertices, int _nbBins)
{
  vector<vector<Eigen::Vector4d>> _bins = vector<vector<Eigen::Vector4d>>(_nbBins, vector<Eigen::Vector4d>());

  Eigen::Vector2d _min_max_norm = MinMaxCoordRadiales(_vertices);

  for(const Eigen::Vector4d& _vertex : _vertices)
  {
    int _indexBin = _nbBins*(_vertex(0) - _min_max_norm(0))/(_min_max_norm(1)-_min_max_norm(0));
    _indexBin = max(0, min(_nbBins-1, _indexBin));
    _bins[_indexBin].push_back(_vertex);
  }
  return _bins;
}

double MeanOfBin(const vector<Eigen::Vector4d>& _bin)
{
  double _total = 0;
  for(const Eigen::Vector4d& _vertex : _bin)
    _total += _vertex(0);
  return _total/(double)_bin.size();
}

void Watermark(vector<vector<Eigen::Vector4d>>& _bins, string _msg, float _alpha, float _delta_k)
{
  for(int i = 0; i<_msg.size(); ++i)
  {
    char _c = _msg[i];
    //Chaque bit du char
    for(int j = 7; j >= 0; --j)
    {
      vector<Eigen::Vector4d>& _bin = _bins[i*8+7-j];
      //Get bit
      int _bit = (_c >> j) & 1;
      
      if(_bit == 1)
      {
        double _mean;
        float _k = 1;
        do{
          if(_bin.size() == 0) break;
          for(Eigen::Vector4d& _vertex : _bin)
              _vertex(0) = pow(_vertex(0), _k);
          _mean = MeanOfBin(_bin);
          _k -= _delta_k;
        }while(_mean < 0.5f + _alpha);
      }
      else if (_bit == 0)
      {
        double _mean;
        float _k = 1;
        do{
          if(_bin.size() == 0) break;
          for(Eigen::Vector4d& _vertex : _bin)
              _vertex(0) = pow(_vertex(0), _k);
          _mean = MeanOfBin(_bin);
          _k += _delta_k;
        }while(_mean > 0.5f - _alpha);
      }
    }
  }
}

Eigen::MatrixXd Tatouage(const Eigen::MatrixXd& _mesh, const string& _msg, float _alpha, float _delta_k)
{
  //Get center
  Eigen::Vector3d _centroid = GravityCenter(_mesh);

  //Get vertices spherical coord
  int _nbVertices = _mesh.rows();
  vector<Eigen::Vector4d> _verticesAndIndex = vector<Eigen::Vector4d>(_nbVertices);
  for(int i = 0; i < _nbVertices; ++i)
  {
    Eigen::Vector3d _coord = CartesianToSpherical(_mesh.row(i), _centroid);
    _verticesAndIndex[i] = Eigen::Vector4d(_coord(0), _coord(1), _coord(2), i);
  }

  //Get nb bins
  unsigned int _nbBins = _msg.length()*8;

  //Sort vertices in related bin due to norm
  vector<vector<Eigen::Vector4d>> _bins = SortInBins(_verticesAndIndex, _nbBins);


  //Normalize bins
  vector<Eigen::Vector2d> _min_max_bins = vector<Eigen::Vector2d>(_bins.size());
      
  for(int i = 0; i < _bins.size(); ++i)
  {
    _min_max_bins[i] = MinMaxCoordRadiales(_bins[i]);
    _bins[i] = Normalize(_bins[i]);
  }

  //Watermark
  Watermark(_bins, _msg, _alpha, _delta_k);

  //UnNormalize bins
  for(int i = 0; i < _bins.size(); ++i)
    _bins[i] = UnNormalize(_bins[i], _min_max_bins[i](0), _min_max_bins[i](1));


  //Unsort and fill new mesh with cartesian
  Eigen::MatrixXd _newMesh = Eigen::MatrixXd(_nbVertices,3);
  for(const vector<Eigen::Vector4d>& _bin : _bins)
    for(const Eigen::Vector4d& _vertex : _bin)
      _newMesh.row(_vertex(3)) = SphericalToCartesian(Eigen::Vector3d(_vertex(0), _vertex(1), _vertex(2)),_centroid);


  return _newMesh;
}

string BinToString(string _binaries)
{
  string _msg = "";
  char _cOut = 0;
  for(int i = 0; i < _binaries.length(); ++i)
  {
    int _binary = _binaries[i] - '0';
    _cOut = (_cOut << 1) | (_binary);
    if((i+1)%8 != 0) continue;
    _msg += _cOut;
    _cOut = 0;
  }
  return _msg;
}

string Extraction(const Eigen::MatrixXd& _mesh, unsigned int _nbChar)
{
  //Get center
  Eigen::Vector3d _centroid = GravityCenter(_mesh);

  //Get vertices spherical coord
  int _nbVertices = _mesh.rows();
  vector<Eigen::Vector4d> _verticesAndIndex = vector<Eigen::Vector4d>(_nbVertices);
  for(int i = 0; i < _nbVertices; ++i)
  {
    Eigen::Vector3d _coord = CartesianToSpherical(_mesh.row(i), _centroid);
    _verticesAndIndex[i] = Eigen::Vector4d(_coord(0), _coord(1), _coord(2), i);
  }

  //Get nb bins
  unsigned int _nbBins = _nbChar*8;

  //Sort vertices in related bin due to norm
  vector<vector<Eigen::Vector4d>> _bins = SortInBins(_verticesAndIndex, _nbBins);

  //Normalize bins      
  for(int i = 0; i < _bins.size(); ++i)
    _bins[i] = Normalize(_bins[i]);

  string _msg;
  //Get binary for each bins
  for(const vector<Eigen::Vector4d>& _bin : _bins)
  {
    float _mean = MeanOfBin(_bin);
    _msg += _mean > 0.5f ? "1" : "0";
  }
  return _msg;
}

double RMSE(const Eigen::MatrixXd& _mesh, const Eigen::MatrixXd& _meshTatoue)
{
    const int _size = _mesh.rows();
    double _SE = 0;
    for(int i = 0; i < _size; ++i)
    {
        const Eigen::Vector3d _meshVertex = _mesh.row(i);
        const Eigen::Vector3d _meshTatoueVertex = _meshTatoue.row(i);
        _SE +=  (_meshVertex(0)-_meshTatoueVertex(0))*(_meshVertex(0)-_meshTatoueVertex(0)) + 
                (_meshVertex(1)-_meshTatoueVertex(1))*(_meshVertex(1)-_meshTatoueVertex(1)) +
                (_meshVertex(2)-_meshTatoueVertex(2))*(_meshVertex(2)-_meshTatoueVertex(2));
    }
    const double _MSE = _SE/(float)_size;
    const double _RMSE = sqrt(_MSE);
    return _RMSE;
}

int main(int argc, char *argv[])
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  // Load a mesh in OFF format
  igl::readOFF("../data/bunny.off", V, F);

  string _msgIn = "Salut";
  Eigen::MatrixXd V2 = Tatouage(V, _msgIn, 0.1f, 0.2f);
  string _msg = Extraction(V2, _msgIn.length());
  double _rmse = RMSE(V, V2);

  cout<<"RMSE : "<<_rmse<<endl;
  string _msgExtrait = BinToString(_msg);
  cout<<"Message Extrait: "<<_msgExtrait<<endl;
  
  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(V2, F);
  viewer.launch();
}
