#include "marcpostdb.h"
#include <Python.h>

#define  CHECKERR(); 	if (PyErr_Occurred()) \
	{  \
		PyErr_Print(); \
		PyErr_Clear(); \
	}

#define  GETFUNC(mod,name,index,message);  	if (m_pFunc[index] == 0) \
	{ \
		if (mod) \
		{ \
			m_pFunc[index] = (void *)PyObject_GetAttrString((PyObject *)mod,name); \
			CHECKERR();  \
			if (m_pFunc[index] == 0)  \
			{  \
				printf("%s\n",message);  \
			}  \
		} \
	}

MarcPostDB::MarcPostDB():m_pFile(0),m_pMod(0)
{
	for(int i = 0; i < FuncCount; i++)
	{
		m_pFunc[i] = 0;
	}
	//Py_SetPythonHome();
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		printf("%s/n","**ERROR** Python initialize failed");
	}
	m_pMod = (void *)PyImport_ImportModule("py_post");
	CHECKERR();
	if (m_pMod == 0)
	{
		printf("%s\n","**ERROR** Module import failed");
	}
}

MarcPostDB *MarcPostDB::getInstance()
{
	static MarcPostDB *db = 0;
	if (db == 0)
	{
		db = new MarcPostDB();
	}
	return db;
}

MarcPostDB::~MarcPostDB()
{
	if (m_pMod != 0)
	{
		Py_DECREF(m_pMod);
		m_pMod = 0;
	}
	Py_Finalize();
}

char* MarcPostDB::version()
{
	GETFUNC(m_pMod,"version",Func_Version,"**ERROR** Function version() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_Version],NULL);
	CHECKERR();
	strcpy_s(m_cStr,128,PyString_AsString(ret));
	Py_DECREF(ret);
	return m_cStr;
}

char* MarcPostDB::title()
{
	GETFUNC(m_pFile,"title",Func_Title,"**ERROR** Function title() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_Title],NULL);
	CHECKERR();
	strcpy_s(m_cStr,128,PyString_AsString(ret));
	Py_DECREF(ret);
	return m_cStr;
}

char* MarcPostDB::extrapolate()
{
	PyObject *obj = PyObject_GetAttrString((PyObject *)m_pFile,"extrapolate");
	strcpy_s(m_cStr,128,PyString_AsString(obj));
	return m_cStr;
}

int MarcPostDB::revision()
{
	PyObject *obj = PyObject_GetAttrString((PyObject *)m_pFile,"revision");
	return PyInt_AsLong(obj);
}

int MarcPostDB::position()
{
	PyObject *obj = PyObject_GetAttrString((PyObject *)m_pFile,"position");
	return PyInt_AsLong(obj);
}

bool MarcPostDB::open(char *fname)
{
	GETFUNC(m_pMod,"post_open",Func_PostOpen,"**ERROR** Function post_open() not found");
	PyObject *pParam = Py_BuildValue("(s)",fname);
	m_pFile = (void *)PyEval_CallObject((PyObject *)m_pFunc[Func_PostOpen],pParam);
	CHECKERR();
	Py_DECREF(pParam);
	if (m_pFile == 0)
	{
		printf("%s\n","**ERROR** DB open failed");
		return false;
	}
	return true;
}

void MarcPostDB::close()
{
	GETFUNC(m_pFile,"close",Func_Close,"**ERROR** Function close() not found");
	PyEval_CallObject((PyObject *)m_pFunc[Func_Close],NULL);
	if (m_pFile != 0)
	{
		Py_DECREF(m_pFile);
		m_pFile = 0;
	}
	for (int i = 0; i < FuncCount; i++)
	{
		if (m_pFunc[i] != 0)
		{
			Py_DECREF(m_pFunc[i]);
			m_pFunc[i] = 0;
		}
	}
}

void MarcPostDB::moveto(int i)
{
	GETFUNC(m_pFile,"moveto",Func_MoveTo,"**ERROR** Function moveto() not found");
	PyObject *pParam = Py_BuildValue("(i)",i);
	PyEval_CallObject((PyObject *)m_pFunc[Func_MoveTo],pParam);
	CHECKERR();
	Py_DECREF(pParam);
}

void MarcPostDB::node(int index,PyNode *pNode)
{
	GETFUNC(m_pFile,"node",Func_Node,"**ERROR** Function node() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_Node],pParam);
	CHECKERR();
	PyObject *val = PyObject_GetAttrString(ret,"id");
	pNode->id = PyInt_AsLong(val);
	Py_DECREF(val);

	val = PyObject_GetAttrString(ret,"x");
	*(pNode->coord)= PyFloat_AsDouble(val);
	Py_DECREF(val);

	val = PyObject_GetAttrString(ret,"y");
	*(pNode->coord+1)= PyFloat_AsDouble(val);
	Py_DECREF(val);

	val = PyObject_GetAttrString(ret,"z");
	*(pNode->coord+2)= PyFloat_AsDouble(val);
	Py_DECREF(val);

	Py_DECREF(pParam);
	Py_DECREF(ret);
}

int MarcPostDB::nodes()
{
	GETFUNC(m_pFile,"nodes",Func_Nodes,"**ERROR** Function nodes() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_Nodes],NULL);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	return i;
}

int MarcPostDB::node_id(int index)
{
	GETFUNC(m_pFile,"node_id",Func_NodeId,"**ERROR** Function node_id() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_NodeId],pParam);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	Py_DECREF(pParam);
	return i;
}

double MarcPostDB::node_scalar(int indexN,int indexS)
{
	GETFUNC(m_pFile,"node_scalar",Func_NodeScalar,"**ERROR** Function node_scalar() not found");
	PyObject *pParam = Py_BuildValue("(i,i)",indexN,indexS);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_NodeScalar],pParam);
	CHECKERR();
	double d = PyFloat_AsDouble(ret);
	Py_DECREF(ret);
	Py_DECREF(pParam);
	return d;
}

int MarcPostDB::node_scalars()
{
	GETFUNC(m_pFile,"node_scalars",Func_NodeScalars,"**ERROR** Function node_scalars() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_NodeScalars],NULL);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	return i;
}

char* MarcPostDB::node_scalar_label(int index)
{
	GETFUNC(m_pFile,"node_scalar_label",Func_NodeScalarLabel,"**ERROR** Function node_scalar_label() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_NodeScalarLabel],pParam);
	CHECKERR();
	strcpy_s(m_cStr,128,PyString_AsString(ret));
	Py_DECREF(ret);
	Py_DECREF(pParam);
	return m_cStr;
}

void MarcPostDB::node_vector(int indexN,int indexV,PyVector *pVec)
{
	GETFUNC(m_pFile,"node_vector",Func_NodeVector,"**ERROR** Function node_vector() not found");
	PyObject *pParam = Py_BuildValue("(i,i)",indexN,indexV);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_NodeVector],pParam);
	CHECKERR();
	PyObject *val = PyObject_GetAttrString(ret,"id");
	pVec->id = PyInt_AsLong(val);
	Py_DECREF(val);

	val = PyObject_GetAttrString(ret,"x");
	*(pVec->val)= PyFloat_AsDouble(val);
	Py_DECREF(val);

	val = PyObject_GetAttrString(ret,"y");
	*(pVec->val+1)= PyFloat_AsDouble(val);
	Py_DECREF(val);

	val = PyObject_GetAttrString(ret,"z");
	*(pVec->val+2)= PyFloat_AsDouble(val);
	Py_DECREF(val);

	Py_DECREF(ret);
	Py_DECREF(pParam);
}

int MarcPostDB::node_vectors()
{
	GETFUNC(m_pFile,"node_vectors",Func_NodeVectors,"**ERROR** Function node_vectors() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_NodeVectors],NULL);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	return i;
}

char* MarcPostDB::node_vector_label(int index)
{
	GETFUNC(m_pFile,"node_vector_label",Func_NodeVectorLabel,"**ERROR** Function node_vector_label() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_NodeVectorLabel],pParam);
	CHECKERR();
	strcpy_s(m_cStr,128,PyString_AsString(ret));
	Py_DECREF(ret);
	Py_DECREF(pParam);
	return m_cStr;
}

void MarcPostDB::element(int index,PyElement *pElement)
{
	GETFUNC(m_pFile,"element",Func_Element,"**ERROR** Function element() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_Element],pParam);
	CHECKERR();

	PyObject *val = PyObject_GetAttrString(ret,"id");
	pElement->id = PyInt_AsLong(val);
	Py_DECREF(val);
	CHECKERR();

	val = PyObject_GetAttrString(ret,"type");
	pElement->elemType = PyInt_AsLong(val);
	Py_DECREF(val);

	val = PyObject_GetAttrString(ret,"len");
	pElement->nodeCnt = PyInt_AsLong(val);
	Py_DECREF(val);

	val = PyObject_GetAttrString(ret,"items");
	if (PyList_Check(val))
	{
		int len = PyList_Size(val);
		PyObject *item = 0;
		for (int i = 0; i < len; i++)
		{
			item = PyList_GetItem(val,i);
			pElement->nodeId[i] = PyInt_AsLong(item);
			//Py_DECREF(item);
		}
	}
	Py_DECREF(val);
	CHECKERR();

	Py_DECREF(ret);
	Py_DECREF(pParam);
}

int MarcPostDB::elements()
{
	GETFUNC(m_pFile,"elements",Func_Elements,"**ERROR** Function elements() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_Elements],NULL);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	return i;
}

int MarcPostDB::element_id(int index)
{
	GETFUNC(m_pFile,"element_id",Func_ElementId,"**ERROR** Function element_id() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementId],pParam);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	Py_DECREF(pParam);
	return i;
}

int MarcPostDB::element_scalar(int indexE,int indexS,PyScalar **ppScalar)
{
	GETFUNC(m_pFile,"element_scalar",Func_ElementScalar,"**ERROR** Function element_scalar() not found");
	PyObject *pParam = Py_BuildValue("(i,i)",indexE,indexS);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementScalar],pParam);
	CHECKERR();

	int len = 0;
	if (PyList_Check(ret))
	{
		len = PyList_Size(ret);
		PyObject *item = 0;
		for (int i = 0; i < len; i++)
		{
			item = PyList_GetItem(ret,i);

			PyObject *val = PyObject_GetAttrString(item,"id");
			m_oScalar[i].nId = PyInt_AsLong(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"value");
			m_oScalar[i].val = PyFloat_AsDouble(val);
			Py_DECREF(val);

			//Py_DECREF(item);
		}
	}
	Py_DECREF(ret);
	Py_DECREF(pParam);
	*ppScalar = m_oScalar;
	return len;
}

int MarcPostDB::element_scalars()
{
	GETFUNC(m_pFile,"element_scalars",Func_ElementScalars,"**ERROR** Function element_scalars() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementScalars],NULL);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	return i;
}

char* MarcPostDB::element_scalar_label(int index)
{
	GETFUNC(m_pFile,"element_scalar_label",Func_ElementScalarLabel,"**ERROR** Function element_scalar_label() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementScalarLabel],pParam);
	CHECKERR();
	strcpy_s(m_cStr,128,PyString_AsString(ret));
	Py_DECREF(ret);
	Py_DECREF(pParam);
	return m_cStr;
}

int MarcPostDB::element_vector(int indexE,int indexV,PyVector **ppVector)
{
	GETFUNC(m_pFile,"element_vector",Func_ElementVector,"**ERROR** Function element_vector() not found");
	PyObject *pParam = Py_BuildValue("(i,i)",indexE,indexV);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementVector],pParam);
	CHECKERR();

	int len = 0;
	if (PyList_Check(ret))
	{
		len = PyList_Size(ret);
		PyObject *item = 0;
		for (int i = 0; i < len; i++)
		{
			item = PyList_GetItem(ret,i);

			PyObject *val = PyObject_GetAttrString(item,"id");
			m_oVector[i].id = PyInt_AsLong(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"x");
			m_oVector[i].val[0] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"y");
			m_oVector[i].val[1] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"z");
			m_oVector[i].val[2] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			//Py_DECREF(item);
		}
	}
	Py_DECREF(ret);
	Py_DECREF(pParam);
	*ppVector = m_oVector;
	return len;
}

int MarcPostDB::element_vectors()
{
	GETFUNC(m_pFile,"element_vectors",Func_ElementVectors,"**ERROR** Function element_vectors() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementVectors],NULL);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	return i;
}

char* MarcPostDB::element_vector_label(int index)
{
	GETFUNC(m_pFile,"element_vector_label",Func_ElementVectorLabel,"**ERROR** Function element_vector_label() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementVectorLabel],pParam);
	CHECKERR();
	strcpy_s(m_cStr,128,PyString_AsString(ret));
	Py_DECREF(ret);
	Py_DECREF(pParam);
	return m_cStr;
}

int MarcPostDB::element_tensor(int indexE,int indexT,PyTensor **ppTensor)
{
	GETFUNC(m_pFile,"element_tensor",Func_ElementTensor,"**ERROR** Function element_tensor() not found");
	PyObject *pParam = Py_BuildValue("(i,i)",indexE,indexT);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementTensor],pParam);
	CHECKERR();

	int len = 0;
	if (PyList_Check(ret))
	{
		len = PyList_Size(ret);
		PyObject *item = 0;
		for (int i = 0; i < len; i++)
		{
			item = PyList_GetItem(ret,i);

			PyObject *val = PyObject_GetAttrString(item,"id");
			m_oTensor[i].nodeId = PyInt_AsLong(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"intensity");
			m_oTensor[i].intensity = PyFloat_AsDouble(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"t11");
			m_oTensor[i].val[0] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"t12");
			m_oTensor[i].val[1] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"t13");
			m_oTensor[i].val[2] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"t22");
			m_oTensor[i].val[3] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"t23");
			m_oTensor[i].val[4] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			val = PyObject_GetAttrString(item,"t33");
			m_oTensor[i].val[5] = PyFloat_AsDouble(val);
			Py_DECREF(val);

			//Py_DECREF(item);
		}
	}
	Py_DECREF(ret);
	Py_DECREF(pParam);
	*ppTensor = m_oTensor;
	return len;
	//printf("%s\n",PyString_AsString(PyObject_Str(ret)));
	//if (PyTuple_Check(ret))
	//{
	//	printf("%s\n","Type of tuple");
	//}
	//else if (PyList_Check(ret))
	//{
	//	printf("%s\n","Type of list");
	//}
	//else if (PyDict_Check(ret))
	//{
	//	printf("%s\n","Type of dict");
	//}
	//else if (PyString_Check(ret))
	//{
	//	printf("%s\n","Type of string");
	//}
	//else if (PySet_Check(ret))
	//{
	//	printf("%s\n","Type of set");
	//}
	//else if (PySequence_Check(ret))
	//{
	//	printf("%s\n","Type of sequence");
	//}
	//else if (PyMapping_Check(ret))
	//{
	//	printf("%s\n","Type of map");
	//}
}

int MarcPostDB::element_tensors()
{
	GETFUNC(m_pFile,"element_tensors",Func_ElementTensors,"**ERROR** Function element_tensors() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementTensors],NULL);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	return i;
}

char* MarcPostDB::element_tensor_label(int index)
{
	GETFUNC(m_pFile,"element_tensor_label",Func_ElementTensorLabel,"**ERROR** Function element_tensor_label() not found");
	PyObject *pParam = Py_BuildValue("(i)",index);
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_ElementTensorLabel],pParam);
	CHECKERR();
	strcpy_s(m_cStr,128,PyString_AsString(ret));
	Py_DECREF(ret);
	Py_DECREF(pParam);
	return m_cStr;
}

int MarcPostDB::increments()
{
	GETFUNC(m_pFile,"increments",Func_Increments,"**ERROR** Function increments() not found");
	PyObject * ret = PyEval_CallObject((PyObject *)m_pFunc[Func_Increments],NULL);
	CHECKERR();
	int i = PyInt_AsLong(ret);
	Py_DECREF(ret);
	return i;
}

void MarcPostDB::extrapolation(char *method)
{
	GETFUNC(m_pFile,"extrapolation",Func_Extrapolation,"**ERROR** Function extrapolation() not found");
	PyObject *pParam = Py_BuildValue("(s)",method);
	PyEval_CallObject((PyObject *)m_pFunc[Func_Extrapolation],pParam);
	Py_DECREF(pParam);
}