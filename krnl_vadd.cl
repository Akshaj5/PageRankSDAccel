#define N 128
#define d 0.85

__kernel void __attribute__ ((reqd_work_group_size(1, 1, 1)))
krnl_vadd(
    __global int* source,
    __global int* dest,
    __global int* out,
    __global int* new_pr,
    __global float* partial_page_rank,
    const int count,
    const int length) {

  int V = count;
  int result[N];
  int iterations = length/N;
  for(int i=0;i < iterations;i++){
    int j;
    read_source:
        __attribute__((xcl_pipeline_loop))
        for(j=0; j < N; j++)
          result[j] = source[i*N+j];

    read_source_write_partial_page_rank: 
       __attribute__((xcl_pipeline_loop))
        for(j=0; j < N; j++)
          partial_page_rank[source[i*N+j]] = float(d*(page_rank[source[i*N+j]]/out[source[i*N+ j]]));
   }
    write_new_pr: 
       __attribute__((xcl_pipeline_loop))
        for(j=0; j < length; j++)
          new_pr[source[j]] = new_pr[source[j]] + partial_page_rank[source[j]];                    
  return;
}