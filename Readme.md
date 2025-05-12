<h2>This Custom 3D Renderer and CPU Rasterizer with features fully implemented by myself</h2>  
  
Utilizing BMPs to act as a sort of virtual screen, I have replicated the rasterization process that 3d graphics software achieves    
  
1.) Allows creation and modification of bmp files  
2.) Displays the bmp file updating every frame 
3.) Allows for the projection of 3d points onto the 2d screen space that is the bmp file 
4.) Supports triple buffer rendering for better animated experience  
5.) Supports linearly interpolation between points to fill in both the colors and pixels in between vertices in a triangle  
6.) Utilizes frame buffer and depth buffer for better performance  
7.) Supports texturing mapping onto triangles  
  
<h2>Examples</h2>  


<h3>Color Intepolation</h3>  

![Gif Spinning Cube](https://github.com/Jakemuzy/BMP-Rasterizer-and-3D-projection/blob/2f71ae98a9a1864ee6e238c3a908be55a974726d/Examples/rotateCube.gif)  
    
<h3>Texture Mapping</h3>  
  
![Gif Textured Cube](https://github.com/Jakemuzy/Custom-3D-Renderer/blob/6765d42c52a4d9916c5f8803def7612aba536b38/Examples/textureCube.gif)       
