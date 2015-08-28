Name: ganttmodellib
Version: 1.0.0
Release: 1
Summary: Библиотека для построения диаграммы Ганта
Group: nk/kpohi
License: proprietary
Source0: ../src/ganttmodel.tar.gz
BuildArch: x86_64
BuildRoot: %{_tmppath}/ganttmodel-root

%description
Библиотека для построения диаграммы Ганта

%package devel
Summary:Библиотека для построения диаграммы Ганта для разработки
Group: nk/kpohi 

%description devel
иблиотека для построения диаграммы Ганта для разработки

%prep
%setup -q -n ganttmodel

%build
qmake4 CONFIG+=build_all ganttmodel.pro
make all


%install
INSTALL_ROOT=$RPM_BUILD_ROOT make install

%files
%defattr(-,root,root)
/usr/lib64/libganttmodel.so*

%files devel
%defattr(-,root,root)
/usr/lib64/libganttmodeld.so*
/usr/include/*

%clean
make clean
rm -rf $RPM_BUILD_ROOT

%post
ldconfig

%changelog
* Fri Apr 1 2013 Lukyanchikov
- Test RPM packet created.
