#version 130                                               
attribute vec4 coord;                                      
out vec2 uv_coord;                                          
void main(){                                                
	uv_coord = coord.zw;                                    
	gl_Position = vec4(coord.xy, 0, 1);           
}