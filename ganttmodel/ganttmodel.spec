Name: ganttmodel
Version: 0
Release: 0
Summary: ���������� ��� ���������� ��������� �����
Group: nk/kpohi
License: proprietary
Source0: ../src/ganttmodel.tar.gz
BuildArch: x86_64
BuildRoot: %{_tmppath}/ganttmodel-root

%description
���������� ��� ���������� ��������� �����

%package dev
Summary:���������� ��� ���������� ��������� ����� ��� ����������
Group: nk/kpohi 

%description dev
��������� ��� ���������� ��������� ����� ��� ����������

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

%files dev
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
