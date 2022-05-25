#pragma once
#include<cstdlib>
#include<memory>
using u_char = unsigned char;
using ngx_uint_t = unsigned int;
struct ngx_pool_s; //����ǰ��˵��

/*��������(�ص�����)������*/
typedef void (*ngx_pool_cleanup_pt)(void* data);
struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler; //����һ������ָ�룬�������������Ļص�����
    void* data; //��ַ���ݸ��ص������Ĳ���
    ngx_pool_cleanup_s* next;   //���е�cleanup����������������һ��������
};

/*����ڴ��ͷ����Ϣ*/
struct ngx_pool_large_s {
    ngx_pool_large_s* next; //���еĴ���ڴ����Ҳ�Ǳ�����һ��������
    void* alloc;    //��������ȥ�Ĵ���ڴ����ʼ��ַ
};

/*����С���ڴ���ڴ�ص�ͷ��������Ϣ*/
struct ngx_pool_data_t {
    u_char* last;   //С���ڴ�ؿ����ڴ���ʼ��ַ
    u_char* end;    //С���ڴ�ؿ����ڴ��ĩβ��ַ
    ngx_pool_s*  next;  //����С���ڴ�ض�������һ��������
    ngx_uint_t      failed; //��¼�˵�ǰС���ڴ�ط���ʧ�ܵĴ���
};

/*ngx�ڴ�ص�ͷ����Ϣ�͹�����Ա��Ϣ*/
struct ngx_pool_s {
    ngx_pool_data_t       d;   //�洢��ǰС���ڴ�Ĵ洢���
    size_t                max;      //�洢С���ڴ�ʹ���ڴ�ķֽ���
    ngx_pool_s* current;      //ָ���һ�������ṩС���ڴ�����С���ڴ��
    ngx_pool_large_s* large;//ָ�����ڴ棨����������ڵ�ַ
    ngx_pool_cleanup_s* cleanup;//ָ������Ԥ�õ����������ص������������������
};

//buf������memset����
#define ngx_memzero(buf, n)       (void) memset(buf, 0, n)
//  ����ֵ d �������ٽ��� a �ı���
#define ngx_align(d, a)     ( ( (d) + (a - 1) ) & ~(a - 1) )
//  С���ڴ���俼���ֽڶ���ʱ�ĵ�λ
#define NGX_ALIGNMENT   sizeof(unsigned long)    /* platform word */
//  ��ָ��p������a���ٽ��ı���
#define ngx_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

//  Ĭ��һ������ҳ��Ĵ�С
const int ngx_pagesize = 4096;
//  ngxС���ڴ�ؿɷ�������ռ�4096-1
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1; 
//  ���峣������ʾһ��Ĭ�ϵ�ngx�ڴ�ؿ��ٵĴ�С
const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;//16k
//  �ڴ�ش�С����16�ֽڽ��ж��루�ڴ���룩
const int NGX_POOL_ALIGNMENT = 16;
//  ngxС���ڴ����С��size������NGX_POOL_ALIGNMENT�ı���
const int NGX_MIN_POOL_SIZE =
    ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)),
        NGX_POOL_ALIGNMENT);

/*
��ֲnginx�ڴ�ش��룬��oop��ʵ��
*/
class ngx_mem_pool {
public:
    //  ����ָ��size��С���ڴ�أ�����С���ڴ�ش�С������һ��ҳ���С
    void* ngx_create_pool(size_t size);
    //  �����ڴ��ֽڶ��룬���ڴ������size��С���ڴ�
    void* ngx_palloc(size_t size);
    //  �������ڴ��ֽڶ��룬���ڴ������size��С���ڴ�
    void* ngx_pnalloc(size_t size);
    // ���õ���ngx_pallocʵ���ڴ���䣬���ǻ��ʼ��0
    void* ngx_pcalloc(size_t size);
    //  �ͷŴ���ڴ�
    void ngx_pfree(void* p);
    //  �ڴ����ú���
    void ngx_reset_pool();
    //  �ڴ�����ٺ���
    void ngx_destroy_pool();
    //  ���ӻص�������������
    ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);

private:
	ngx_pool_s * pool;	//ָ��nginx�ڴ�����ָ��
    
            
    //  С���ڴ����
    void* ngx_palloc_small(size_t size, ngx_uint_t align);
    //  �����µ�С���ڴ��
    void* ngx_palloc_block(size_t size);
    //  ����ڴ����
    void* ngx_palloc_large(size_t size);
    
};