void getTBN(vec3 normal, vec3 tangent, mat3 M, out vec3 T, out vec3 B, out mat3 TBN)
{
  vec3 N = normal;
  T = normalize(mat3(M) * tangent);
  B = cross(T, N);
  TBN = mat3(T, B, N); 
}
