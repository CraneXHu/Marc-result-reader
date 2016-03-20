#ifndef MARCPOSTDB_H
#define MARCPOSTDB_H

enum Function
{
	Func_Version,
	Func_Title,
	Func_PostOpen,
	Func_Close,
	Func_MoveTo,
	Func_Node,
	Func_Nodes,
	Func_NodeId,
	Func_NodeScalar,
	Func_NodeScalars,
	Func_NodeScalarLabel,
	Func_NodeVector,
	Func_NodeVectors,
	Func_NodeVectorLabel,
	Func_Element,
	Func_Elements,
	Func_ElementId,
	Func_ElementScalar,
	Func_ElementScalars,
	Func_ElementScalarLabel,
	Func_ElementVector,
	Func_ElementVectors,
	Func_ElementVectorLabel,
	Func_ElementTensor,
	Func_ElementTensors,
	Func_ElementTensorLabel,
	Func_Increments,
	Func_Extrapolation,
	FuncCount = 64,
};

struct PyNode
{
	int id;
	double coord[3];
};

struct PyScalar
{
	int nId;
	double val;
};

struct PyVector
{
	int id;
	double val[3];
};

struct PyElement
{
	int id;
	int elemType;
	int nodeCnt;
	int nodeId[20];
};

struct PyTensor
{
	int nodeId;
	double intensity;
	double val[6];
};

class __declspec(dllexport) MarcPostDB
{
public:
	static MarcPostDB *getInstance();
	~MarcPostDB();
	
	char* version();
	char* title();
	int revision();
	bool open(char *fname);
	void close();
	char* extrapolate();
	void moveto(int i);
	int position();
	void node(int index,PyNode *pNode);
	int nodes();
	int node_id(int index);
	void node_vector(int indexN,int indexV,PyVector *pVec);
	int node_vectors();
	char* node_vector_label(int index);
	double node_scalar(int indexN,int indexS);
	int node_scalars();
	char* node_scalar_label(int index);
	void element(int index,PyElement *pElement);
	int elements();
	int element_id(int index);
	int element_scalar(int indexE,int indexS,PyScalar **ppScalar);
	int element_scalars();
	char* element_scalar_label(int index);
	int element_vector(int indexE,int indexV,PyVector **ppVector);
	int element_vectors();
	char* element_vector_label(int index);
	int element_tensor(int indexE,int indexT,PyTensor **ppTensor);
	int element_tensors();
	char* element_tensor_label(int index);
	int increments();
	void extrapolation(char *method); //"linear"  "translate"  "average"

private:
	MarcPostDB();

private:
	void *m_pFile, *m_pMod, *m_pFunc[FuncCount];
	char m_cStr[128];
	PyScalar m_oScalar[20];
	PyVector m_oVector[20];
	PyTensor m_oTensor[20];
};

#endif //MARCPOSTDB_H