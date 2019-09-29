
int luaopen_matrix(lua_State *L);
int luaopen_vector(lua_State *L);

matrix_t *lua_newmatrix(lua_State *L);
vector_t *lua_newvector(lua_State *L);

int lua_isvector(lua_State *L, int index);
vector_t *lua_tovectorptrx(lua_State *L, int index, int *is_vector);
vector_t *lua_tovectorptr(lua_State *L, int index);
vector_t lua_tovectorx(lua_State *L, int index, int *is_vector);
vector_t lua_tovector(lua_State *L, int index);

int lua_ismatrix(lua_State *L, int index);
matrix_t *lua_tomatrixptrx(lua_State *L, int index, int *is_matrix);
matrix_t *lua_tomatrixptr(lua_State *L, int index);
matrix_t lua_tomatrixx(lua_State *L, int index, int *is_matrix);
matrix_t lua_tomatrix(lua_State *L, int index);

int _G_matrix_toMatrix(lua_State *L);
int _G_matrix_new(lua_State *L);
int _G_matrix_newTranslate(lua_State *L);
int _G_matrix_newScale(lua_State *L);
int _G_matrix_newRotate(lua_State *L);
int _G_matrix_newOrthographic(lua_State *L);
int _G_matrix_newFrustrum(lua_State *L);
int _G_matrix_newPerspective(lua_State *L);
int _G_matrix_newLookAt(lua_State *L);
int _G_matrix_identity(lua_State *L);
int _G_matrix_empty(lua_State *L);
int _G_matrix_negate(lua_State *L);
int _G_matrix_index(lua_State *L);
int _G_matrix_new_index(lua_State *L);
int _G_matrix_plus(lua_State *L);
int _G_matrix_minus(lua_State *L);
int _G_matrix_times(lua_State *L);
int _G_matrix_det(lua_State *L);
int _G_matrix_trace(lua_State *L);
int _G_matrix_transpose(lua_State *L);
int _G_matrix_inverse(lua_State *L);
int _G_matrix_equals(lua_State *L);
int _G_matrix_toString(lua_State *L);
int _G_matrix_isMatrix(lua_State *L);

int _G_vector_toVector(lua_State *L);
int _G_vector_new(lua_State *L);
int _G_vector_index(lua_State *L);
int _G_vector_new_index(lua_State *L);
int _G_vector_empty(lua_State *L);
int _G_vector_negate(lua_State *L);
int _G_vector_magnitude(lua_State *L);
int _G_vector_homo(lua_State *L);
int _G_vector_three(lua_State *L);
int _G_vector_mean(lua_State *L);
int _G_vector_distance(lua_State *L);
int _G_vector_plus(lua_State *L);
int _G_vector_minus(lua_State *L);
int _G_vector_times(lua_State *L);
int _G_vector_dot(lua_State *L);
int _G_vector_cross(lua_State *L);
int _G_vector_normalize(lua_State *L);
int _G_vector_equals(lua_State *L);
int _G_vector_toString(lua_State *L);
int _G_vector_clamp(lua_State *L);
int _G_vector_reflect(lua_State *L);
int _G_vector_isVector(lua_State *L);




