/**
 * This file is part of
 * SSAGES - Suite for Advanced Generalized Ensemble Simulations
 *
 * Copyright 2016 Julian Helfferich <julian.helfferich@gmail.com>
 *
 * SSAGES is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SSAGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SSAGES.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <vector>

#include "Drivers/DriverException.h"
#include "JSON/Serializable.h"
#include "schema.h"
#include "Validator/ObjectRequirement.h"

// Forward declare.
namespace Json {
    class Value;
}

namespace SSAGES
{

//! Generic Grid.
/*!
 * \tparam T type of data to be stored in the Grid.
 *
 * A Grid is a general method to store data in SSAGES. It is used to discretize
 * a continuous number, typically a collective variable, into \c number_points
 * grid points. For each grid point an arbitrary type of data can be stored,
 * specified via the template parameter \c T.
 *
 * The Grid can be of arbitrary dimension. For each dimension, the lower bound,
 * the upper bound and the number of grid points need to be specified.
 * Furthermore, the grid can be defined as periodic or non-periodic in the
 * respective dimension. By default, the grid is non-periodic. The grid points
 * are indexed from 0 to number_points-1 following the standard C/C++
 * convention. The indices -1 and and \c number_points can be used to access
 * the underflow and overflow intervals (see below).
 *
 * The grid spacing \c Delta is given by (upper - lower)/number_points. Thus,
 * grid point \c n corresponds to the interval
 * [lower + n*Delta, lower + (n+1)*Delta) and the position of the grid point is
 * at the center of this interval, at lower + (n+0.5)*Delta. Note that n
 * follows the C/C++ convention, i.e. n = 0 for the first interval. The grid
 * indices pertaining to a given point can be obtained via Grid::GetIndices().
 *
 * In non-periodic dimensions, an overflow and an underflow interval exist. The
 * underflow interval corresponds to the interval (-infinity, lower), i.e. all
 * points below \c lower. Similarly, the overflow interval corresponds to the
 * interval [upper, infinity). The underflow grid point can be accessed via the
 * index -1, the overflow grid point via the index \c number_points.
 *
 * \ingroup Core
 */
template<typename T>
class Grid : public Serializable
{
private:
    //! Number of points in each dimension.
    std::vector<size_t> _numPoints;

    //! Edges of the Grid in each dimension.
    std::pair< std::vector<double>,std::vector<double> > _edges;

    //! Periodicity of the Grid.
    std::vector<bool> _isPeriodic;

    //! Internal storage of the data
    std::vector<T> _data;

    //! Constructor
    /*!
     * \param numPoints Number of grid points in each dimension.
     * \param lower Lower edges of the grid.
     * \param upper Upper edges of the grid.
     *
     * The constructor is intentionally private to make sure, that it is only
     * called via BuildGrid().
     *
     * The dimension of the grid is determined by the size of the parameter
     * vectors.
     */
    Grid(std::vector<size_t> numPoints,
         std::vector<double> lower,
         std::vector<double> upper)
      : _numPoints(numPoints),
        _edges(std::pair< std::vector<double>, std::vector<double> >(lower, upper))
    {
    }

public:
    //! Get the dimension.
    size_t GetDimension() const
    {
        return _numPoints.size();
    }

    //! Get the number of points for all dimensions.
    std::vector<size_t> GetNumPoints() const
    {
        return _numPoints;
    }

    //! Get the number of points for a specific dimension.
    /*!
     * \param dim Index of the dimension.
     *
     * \note The first dimension uses the index 0.
     */
    size_t GetNumPoints(size_t dim) const
    {
        if (dim >= GetDimension()) {
            std::cerr << "Warning! Grid size requested for a dimension larger "
                         "than the grid dimensionality!\n";
            return 0;
        }

        return _numPoints.at(dim);
    }

    //! Return the lower edges of the Grid.
    std::vector<double> GetLower() const
    {
        return _edges.first;
    }

    //! Get the lower edge for a specific dimension.
    /*!
     * \param dim Index of the dimension.
     *
     * \note The first dimension has the index 0.
     */
    double GetLower(size_t dim) const
    {
        if (dim >= GetDimension()) {
            std::cerr << "Warning! Lower edge requested for a dimension larger "
                         "than the grid dimensionality!\n";
            return 0.0;
        }
        return GetLower().at(dim);
    }

    //! Return the upper edges of the Grid.
    std::vector<double> GetUpper() const
    {
        return _edges.second;
    }


    //! Get the upper edge for a specific dimension.
    /*!
     * \param dim Index of the dimension.
     *
     * \note The dimensions are indexed starting with 0.
     */
    double GetUpper(size_t dim) const
    {
        if (dim >= GetDimension()) {
            std::cerr << "Warning! Upper edge requested for a dimension larger "
                         "than the grid dimensionality!\n";
            return 0.0;
        }
        return GetUpper().at(dim);
    }

    //! Return the periodicity of the Grid.
    std::vector<bool> GetPeriodic() const
    {
        return _isPeriodic;
    }

    //! Get the periodicity in a specific dimension.
    /*!
     * \param dim Index of the dimension.
     *
     * \note The dimensions are indexed starting with 0.
     */
    bool GetPeriodic(size_t dim) const
    {
        if (dim >= GetDimension()) {
            std::cerr << "Warning! Periodicity requested for a dimension larger "
                         "than the grid dimensionality!\n";
            return false;
        }
        return GetPeriodic().at(dim);
    }

    //! Return the Grid indices for a given point.
    std::vector<int> GetIndices(std::vector<double> x) const
    {
        return std::vector<int>(GetDimension(), 0);
    }

    //! Access Grid element read-only
    /*!
     * \param indices Vector of integers specifying the grid point.
     */
    const T& at(std::vector<int> indices) const
    {
        return _data.at(0);
    }

    //! Access Grid element read/write
    /*!
     * \param indices Vector of integers specifying the grid point.
     */
    T& at(std::vector<int> indices)
    {
        return _data.at(0);
    }

    //! Access Grid element pertaining to a specific point -- read-only
    /*!
     * \param x Vector of doubles specifying a point.
     *
     * This function is provided for convenience. It is identical to
     * Grid::at(Grid::GetIndices(x)).
     */
    const T& at(std::vector<double> x) const
    {
        return _data.at(GetIndices(x));
    }

    //! Access Grid element pertaining to a specific point -- read/write
    /*!
     * \param x Vector of doubles specifying a point.
     *
     * This function is provided for convenience. It is identical to
     * Grid::at(Grid::GetIndices(x)).
     */
    T& at(std::vector<double> x)
    {
        return _data.at(GetIndices(x));
    }

    //! Set up the grid
    /*!
     * \param json JSON value containing all input information.
     *
     * This function builds a grid from a JSON node. It will return a nullptr
     * if an unknown error occured, but generally, it will throw a
     * BuildException of failure.
     */
    static Grid<T>* BuildGrid(const Json::Value& json)
    {
        return BuildGrid(json, "#/Grid");
    }

    //! Set up the grid
    /*!
     * \param json JSON Value containing all input information.
     * \param path Path for JSON path specification.
     *
     * This function builds a grid from a JSON node. It will return a nullptr
     * if an unknown error occured, but generally, it will throw a
     * BuildException on failure.
     */
    static Grid<T>* BuildGrid(const Json::Value& json, const std::string& path)
    {
        return nullptr;
    }

    //! \copydoc Serializable::Serialize()
    /*!
     * \warning Serialization not yet implemented.
     */
    void Serialize(Json::Value& json) const override
    {

    }
};

} // End namespace SSAGES
