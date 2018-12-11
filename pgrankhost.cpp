#include "xcl2.hpp"
#include <vector>
#pragma once

#define LENGTH (88234)
#define NUM_WORKGROUPS (1)
#define WORKGROUP_SIZE (16)
#define COUNT (4038)


void calc_max_diff(int* arry,int LENGTH){
	for(int i=0;i<LENGTH)
}


int main(int argc, char* argv[]) {

	char ch, file_name[25];
	FILE *fp;
	char c;

	fp = fopen("finalgraph.txt", "r"); // read mode

	if(fp == NULL){
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	int line=0;
	for(c = getc(fp); c != EOF; c = getc(fp)) 
        if (c == '\n') // Increment count if this character is newline 
            line = line + 1; 

	fclose(fp);
	printf("%d\n",line);
	fp = fopen("finalgraph.txt", "r");

	//int src[LENGTH],des[LENGTH],out[LENGTH],pr[LENGTH];

	size_t vector_size_bytes = sizeof(int) * LENGTH;
    std::vector<int,aligned_allocator<int>> src   (LENGTH);
    std::vector<int,aligned_allocator<int>> des   (LENGTH);
    std::vector<int,aligned_allocator<int>> out (LENGTH);
    size_t pgrank_size_bytes = sizeof(float) * COUNT;
    std::vector<float,aligned_allocator<float>> pr (COUNT);
    std::vector<float,aligned_allocator<float>> new_pr (COUNT);

	int count=0;
	line=88233;
	while(count<line){
		ch=fgetc(fp); 	
		c=ch;
		if (c != '\n'){

				src[count]=0;
				des[count]=0;
				out[count]=0;
				pr[count]=0;
				while(c != ' '){
					int read = c-'0';
					src[count]=( (src[count]*10)+read);
					ch = fgetc(fp);
					c=ch; 
				}

				ch = fgetc(fp);
				c=ch;

				while(c !=' ' ){
					int read = c-'0';
					des[count]=( (des[count]*10)+read);
					ch = fgetc(fp);
					c=ch;
				}

				ch = fgetc(fp);
				c=ch;

				while(c !=' '){ 
					int read = c-'0';
					out[count]=( (out[count]*10)+read);
					ch = fgetc(fp);
					c=ch;
				}
				ch = fgetc(fp);
				c=ch;

				while(c !='\n'){ 
					int read = c-'0';
					pr[src[count]]=((pr[src[count]]*10)+read);
					ch = fgetc(fp);
					c=ch;
				}
			count = count+1;
		}
	}	

	std::vector<cl::Device> devices = xcl::get_xil_devices();
    CL_DEVICE_NAME::Device device = devices[0];

    cl::Context context(device);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);
    std::string device_name = device.getInfo<CL_DEVICE_NAME>();

    std::string binaryFile = xcl::find_binary_file(device_name,"krnl_vadd");
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
	devices.resize(1);

	cl::Program program(context, devices, bins);
    cl::Kernel krnl(program,"krnl_vadd");

    std::vector<cl::Memory> inBufVec, outBufVec;


    cl::Buffer buffer_a(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            vector_size_bytes, src.data());
    cl::Buffer buffer_b(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            vector_size_bytes, des.data());
    cl::Buffer buffer_c(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            vector_size_bytes, out.data());
    cl::Buffer buffer_d(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            vector_size_bytes, pr.data());
    cl::Buffer buffer_e(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            pgrank_size_bytes, new_pr.data());
    
    inBufVec.push_back(buffer_a);
    inBufVec.push_back(buffer_b);
    inBufVec.push_back(buffer_c);
    inBufVec.push_back(buffer_d);
    outBufVec.push_back(buffer_e);

    q.enqueueMigrateMemObjects(inBufVec,0/* 0 means from host*/);

    int threshold = 0.0001; // since the average page rank of all pages will be 1 acc. to this formula.
    int no_of_iter = 50;

    int vector_length = LENGTH;
    int count_length = COUNT;
    int threshold = INTMAX;
    for(int i=0;i<no_of_iter && thresh>threshold;i++){
	    krnl.setArg(0, buffer_a);
	    krnl.setArg(1, buffer_b);
	    krnl.setArg(2, buffer_c);
	    krnl.setArg(3, buffer_d);
	    krnl.setArg(4, buffer_e);
	    krnl.setArg(5, count_length);
	    krnl.setArg(6, vector_length);
			
	    threshold = calc_max_diff(new_pr,COUNT);
	
    	/* Launch the kernel */
    	q.enqueueTask(krnl);

     	/* Copy result to local buffer */
    	q.enqueueMigrateMemObjects(outBufVec,CL_MIGRATE_MEM_OBJECT_HOST);
    	q.finish();
	}


	//Testing part yet to be done.







	return 0;
}