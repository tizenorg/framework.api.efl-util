Name:       capi-ui-efl-util
Summary:    An EFL utility library in Tizen Native API
Version:    0.1.0
Release:    5
Group:      TO_BE/FILLED_IN
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(capi-base-common)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description


%package devel
Summary:  An EFL utility library in Tizen Native API (Development)
Group:    TO_BE/FILLED_IN
Requires: %{name} = %{version}-%{release}

%description devel

%prep
%setup -q


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/libcapi-ui-efl-util.so.*

%files devel
%{_includedir}/ui/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-ui-efl-util.so

