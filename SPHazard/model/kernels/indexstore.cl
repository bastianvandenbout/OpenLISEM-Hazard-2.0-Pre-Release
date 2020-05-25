__kernel                                        
void _storeindices(                    	
            __global int *input_iptr, 
			__global int *input_cptr,			
            __write_only image2d_t T_INDEX,
			const int length,
			const int dim0,
			const int dim1
			)           
{                                    			

	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	
    int tid = min((int)(length-1),max((int)(0),(int)(get_global_id(0))));  		
	int id = min((int)(length-1),max((int)(0),(int)(input_iptr[tid])));
	int idc = input_cptr[id];
	
	
	int tidp = tid-1;
	int idp = tidp > -1? min((int)(length-1),max((int)(0),(int)(input_iptr[tidp]))) : -1;
	int idcp = tidp > -1? input_cptr[idp] : -1;

	
	if(idcp < idc)
	{
		int gx = min(dim0-(int)(1),max((int)(0),(int)(idc % dim0)));
		int gy = min(dim1-(int)(1),max((int)(0),(int)(idc - gx)/dim0));
		write_imagef(T_INDEX, (int2)(gx,gy),tid);
	}
	
}   