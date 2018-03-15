
# systemctl start docker
# docker login
# docker build -t tbaudier/vv .
# docker push tbaudier/vv
# docker pull tbaudier/vv
# docker run -ti --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v /home/tbaudier:/home tbaudier/vv
# docker images
# docker ps -a
# docker rm -f `docker ps -aq `
# docker rmi -f `docker images -q `

FROM opensuse:42.3
RUN zypper install -y cmake \
                      git \
                      gcc \
                      gcc-c++ \
                      libQt5Core-devel \
                      libqt5-qtbase-devel \
                      libqt5-qttools-devel \
                      libqt5-qtx11extras-devel \
                      libXt-devel

#Build VTK
RUN mkdir VTK \
 && cd VTK \
 && mkdir src \
 && mkdir bin \
 && mkdir build \
 && git clone --branch v7.1.0 https://github.com/Kitware/VTK.git src \
 && cd bin \
 && cmake ../src/ -DCMAKE_INSTALL_PREFIX="/VTK/build/" -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DVTK_RENDERING_BACKEND=OpenGL -DVTK_USE_CXX11_FEATURES=ON -DModule_vtkGUISupportQt=ON -DModule_vtkGUISupportQtOpenGL=ON -DModule_vtkRenderingQt=ON -DModule_vtkViewsQt=ON -DVTK_QT_VERSION=5 \
 && make -j4 \
 && make install \
 && cd .. \
 && rm -rf bin src

#Build ITK
RUN mkdir ITK \
 && cd ITK \
 && mkdir src \
 && mkdir bin \
 && mkdir build \
 && git clone --branch v4.13.0 https://github.com/InsightSoftwareConsortium/ITK.git src \
 && cd bin \
 && cmake ../src/ -DCMAKE_INSTALL_PREFIX="/ITK/build/" -DVTK_DIR="/VTK/build/lib/cmake/vtk-7.1" -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DModule_ITKVtkGlue=ON \
 && make -j4 \
 && make install \
 && cd .. \
 && rm -rf bin src

#Build vv
RUN mkdir vv \
 && cd vv \
 && mkdir src \
 && mkdir bin \
 && mkdir build \
 && git clone https://github.com/open-vv/vv.git src \
 && cd bin \
 && cmake ../src/ -DCMAKE_INSTALL_PREFIX="/vv/build/" -DITK_DIR="/ITK/build/lib/cmake/ITK-4.13" -DCLITK_BUILD_TOOLS=ON -DCLITK_BUILD_REGISTRATION=ON -DCLITK_BUILD_SEGMENTATION=ON \
 && make -j4 \
 && make install \
 && cd .. \
 && rm -rf bin src

RUN echo 'PATH=/VTK/build/bin/:/ITK/build/bin/:/vv/build/bin/:${PATH}'>>~/.bashrc \
 && echo 'LD_LIBRARY_PATH=/VTK/build/lib/:/ITK/build/lib/:${LD_LIBRARY_PATH}'>>~/.bashrc \
 && echo 'export LD_LIBRARY_PATH'>>~/.bashrc
RUN source ~/.bashrc
RUN ldconfig -v

ENTRYPOINT [ "bash" ]
