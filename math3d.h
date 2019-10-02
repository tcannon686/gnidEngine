
typedef struct
{
	vecc_t t;
	vector_t position;
	vector_t normal;
	vector_t barycentric;
} hit_t;

typedef struct
{
	vector_t a;
	vector_t b;
	vector_t origin;
} bounding_box_t;

typedef struct ray
{
	vector_t o;
	vector_t d;
	vector_t d_inverse;
} ray_t;

typedef struct triangle
{
	union
	{
		vector_t v[3];
		struct
		{
			vector_t v0;
			vector_t v1;
			vector_t v2;
		};
	
		struct
		{
			vector_t a;
			vector_t b;
			vector_t c;
		};
	};
	
	union
	{
		struct
		{
			vector_t ba;
			vector_t cb;
			vector_t ac;
		};
		
		struct
		{
			vector_t v1v0;
			vector_t v2v1;
			vector_t v0v2;
		};
	};
	
	union
	{
		struct
		{
			vecc_t len_ba;
			vecc_t len_cb;
			vecc_t len_ac;
		};
		
		struct
		{
			vecc_t len_v1v0;
			vecc_t len_v2v1;
			vecc_t len_v0v2;
		};
	};

	vector_t origin;
	vector_t normal;
	
	vecc_t area;
} triangle_t;

void CreateRay(ray_t *ray, vector_t o, vector_t d);
void CreateTriangle(
    triangle_t *triangle,
    vector_t a,
    vector_t b,
    vector_t c);

void CreateBoundingBox(
    bounding_box_t *box,
    vector_t min,
    vector_t max);

void CreateHit(hit_t *hit_ptr);

int Barycentric(vector_t *dest, vector_t *v, triangle_t *tri);
int RayPlane(triangle_t *self, hit_t *hit_ptr, ray_t *ray);
int RayTriangle(triangle_t *self, hit_t *hit_ptr, ray_t *ray);
int RayBox(bounding_box_t *self, hit_t *hit_ptr, ray_t *ray);
vecc_t TrianglePointDistance(
    triangle_t *self,
    vector_t vector,
    vector_t *nearest_pt);
vecc_t TriangleSegmentDistance(
    triangle_t *self,
    vector_t a,
    vector_t b,
    vector_t *nearest_pt,
    vector_t *pt_on_line);

