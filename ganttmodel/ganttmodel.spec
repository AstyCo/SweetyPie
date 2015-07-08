Name: ganttmodel
Version: 0
Release: 0
Summary: Библиотека для построения диаграммы Ганта
Group: nk/kpohi
License: proprietary
Source0: ../src/ganttmodel.tar.gz
BuildArch: x86_64
Requires: LukyanchikovLib
BuildRoot: %{_tmppath}/ganttmodel-root

%description
Библиотека для построения диаграммы Ганта




%prep
%setup -q -n ganttmodel

%build
qmake4 ganttmodel.pro -o Makefile.release
make -f Makefile.release
make clean -f Makefile.release

%install
mkdir -p $RPM_BUILD_ROOT/usr/lib64

cd release
cp -d  libganttmodel*.so* $RPM_BUILD_ROOT/usr/lib64


%files
%defattr(-,root,root)
/usr/lib64/libganttmodel*.so*

%clean
rm -rf $RPM_BUILD_ROOT
%post
ldconfig

%changelog
* Fri Apr 1 2013 Lukyanchikov
- Test RPM packet created.
