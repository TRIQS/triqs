from pytriqs.gf import *                                             
                                                                           
G_iw = GfImFreq(beta=40,indices=[0,1])                                     
G_w = GfReFreq(window=(-3,3),indices=[0,1])                                
                                                                                                          
A_iw = G_iw.copy()
A_w =G_w.copy()                                            
A_iw << 1.0j*G_iw                                                           
A_w << 1.0j*G_w
